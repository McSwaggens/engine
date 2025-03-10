#include "general.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
static VkInstance vk;

#include "math.h"
#include "window.h"

#define LogVar(var) Print("%:%: note: " #var " = %\n", CString(__FILE__), __LINE__, var)

#include "vector.h"
#include "quaternion.h"

#include "list.h"

#include "swapchain.h"

#include "assert.cc"
#include "alloc.cc"
#include "unix.cc"
#include "window.cc"
#include "print.cc"
#include "file_system.cc"
#include "swapchain.cc"

#include "vk_helper.h"

struct Queue {
	VkQueue vk;
	u32 index;
	u32 family;
};

template<typename T, u32 N>
struct FixedAllocator {
	T stack[N];
	u32 head = 0;

	T* Next() {
		Assert(head < N);
		return &stack[head++];
	}

	T* begin() { return stack; }
	T* end()   { return stack + head; }
};

struct QueueFamilyTable {
	u32 graphics = -1;
	u32 compute  = -1;
	u32 transfer = -1;
	u32 present  = -1;

	bool IsComplete() {
		if (graphics == -1) return false;
		if (compute  == -1) return false;
		if (transfer == -1) return false;
		if (present  == -1) return false;
		return true;
	}
};

static VkPhysicalDevice physical_device;
static VkDevice device;
static List<VkLayerProperties> vk_layers;
static List<VkPhysicalDevice> physical_devices;
static List<const char*> vk_enabled_extensions;
static FixedAllocator<Queue, 32> queues;
static Queue* general_queue;
static QueueFamilyTable queue_family_table;
static Window window;
static Swapchain swapchain;
static VkShaderModule vert;
static VkShaderModule frag;
static VkRenderPass renderpass;
static VkPipelineLayout pipeline_layout;
static VkPipeline pipeline;
static VkCommandPool command_pool;

static const char* vk_enabled_layers[] = {
	"VK_LAYER_KHRONOS_validation",
};

static u32 vk_enabled_layer_count = sizeof(vk_enabled_layers) / sizeof(*vk_enabled_layers);

void QueryValidationLayers() {
	u32 layer_count;
	vkEnumerateInstanceLayerProperties(&layer_count, null);

	vk_layers.AssureCount(layer_count);
	vkEnumerateInstanceLayerProperties(&layer_count, vk_layers.elements);

	Print("Layers:\n");
	for (auto layer : vk_layers)
		Print("\t%\n", CString(layer.layerName));

}

static bool IsValidationLayerPresent(String str) {
	for (auto& layer : vk_layers) {
		if (CString(layer.layerName) != str)
			continue;

		return true;
	}

	return false;
}

static List<const char*> QueryGlfwRequiredExtensions() {
	List<const char*> result;
	u32 count = 0;
	result.elements = glfwGetRequiredInstanceExtensions(&count);
	result.count = count;
	result.capacity = 0;
	return result;
}

static void InitVulkan() {
	Print("Initializing vulkan...\n");
	List<const char*> glfw_required_extensions = QueryGlfwRequiredExtensions();

	vk_enabled_extensions.Add(glfw_required_extensions);
	vk_enabled_extensions.Add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#ifdef MACOS
	vk_enabled_extensions.Add(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif


	Print("Required Extensions:\n");
	for (auto ext : vk_enabled_extensions)
		Print("\t%\n", CString(ext));

	Print("Glfw Required Extensions:\n");
	for (auto ext : glfw_required_extensions)
		Print("\t%\n", CString(ext));

	VkApplicationInfo app_info = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "Engine",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "xxx",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_API_VERSION_1_4,
	};

	QueryValidationLayers();

	for (auto layer_name : vk_enabled_layers)
		Assert(IsValidationLayerPresent(CString(layer_name)));

	VkInstanceCreateInfo inst_info = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
#ifdef MACOS
		.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
#endif
		.pApplicationInfo = &app_info,

		.ppEnabledExtensionNames = vk_enabled_extensions.elements,
		.enabledExtensionCount   = vk_enabled_extensions.count,

		.ppEnabledLayerNames = vk_enabled_layers,
		.enabledLayerCount   = vk_enabled_layer_count,
	};

	VkResult result = vkCreateInstance(&inst_info, null, &vk);
	LogVar(ToString(result));
	Assert(result == VK_SUCCESS);
}

static List<VkPhysicalDevice> QueryPhysicalDevices() {
	List<VkPhysicalDevice> result;
	u32 count = 0;
	vkEnumeratePhysicalDevices(vk, &count, null);
	result.AssureCount(count);
	vkEnumeratePhysicalDevices(vk, &count, result.elements);
	result.count = count;
	return result;
}

static bool CanQueueFamilyPresent(VkPhysicalDevice pdev, VkSurfaceKHR surface, u32 family) {
	VkBool32 can_present = false;
	vkGetPhysicalDeviceSurfaceSupportKHR(pdev, family, surface, &can_present);
	return can_present;
}

static QueueFamilyTable QueryQueueFamilyTable(VkPhysicalDevice pdev) {
	u32 count;
	vkGetPhysicalDeviceQueueFamilyProperties(pdev, &count, null);

	VkQueueFamilyProperties queue_props[count];
	vkGetPhysicalDeviceQueueFamilyProperties(pdev, &count, queue_props);

	QueueFamilyTable result;

	u32 all = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
	for (u32 i = 0; i < count; i++) {
		VkQueueFamilyProperties* props = &queue_props[i];

		u32 feature_count = PopCount(props->queueFlags);

		if ((props->queueFlags & VK_QUEUE_GRAPHICS_BIT) && result.graphics == -1)
			result.graphics = i;

		if ((props->queueFlags & VK_QUEUE_COMPUTE_BIT)  && result.compute  == -1)
			result.compute = i;

		if ((props->queueFlags & VK_QUEUE_TRANSFER_BIT) && result.transfer == -1)
			result.transfer = i;

		bool can_present = CanQueueFamilyPresent(pdev, window.surface, i);

		if (can_present && result.present == -1)
			result.present = i;
	}

	return result;
}

static bool IsPhysicalDeviceGood(VkPhysicalDevice pdev) {
	VkPhysicalDeviceProperties props;
	vkGetPhysicalDeviceProperties(pdev, &props);

	// @Todo: Score devices and pick the one with the highest score.
	// Also need to make sure the device selected has the nessesary features present.
	// This is good enough for now.

	if (props.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
		props.deviceType != VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
		return false;

	QueueFamilyTable queue_table = QueryQueueFamilyTable(pdev);
	if (!queue_table.IsComplete())
		return false;

	SwapchainSupportInfo swapchain_info = QuerySwapchainSupportInfo(pdev, window.surface);
	if (!swapchain_info.formats.count || !swapchain_info.present_modes.count) {
		swapchain_info.Free();
		return false;
	}

	Print("Using Physical Device: %\n", CString(props.deviceName));
	swapchain_info.Free();
	return true;
}

static VkPhysicalDevice FindPhysicalDevice() {
	Print("Finding physical device...\n");

	physical_devices = QueryPhysicalDevices();

	for (VkPhysicalDevice pdev : physical_devices)
		if (IsPhysicalDeviceGood(pdev))
			return pdev;

	Assert(false);
	return null;
}

static VkDevice CreateLogicalDevice(VkPhysicalDevice pdev) {
	Print("Creating logical device...\n");

	float priority = 1.0;
	standard_output_buffer.Flush();
	VkDeviceQueueCreateInfo queue_create_info = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = queue_family_table.graphics,
		.queueCount = 1,
		.pQueuePriorities = &priority,
	};

	VkPhysicalDeviceFeatures features = {
	};

	List<const char*> ldev_extension_names;
	ldev_extension_names.Add(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
#ifdef MACOS
	ldev_extension_names.Add("VK_KHR_portability_subset");
#endif

	Print("Device extensions:\n");
	for (const char* ext_name : ldev_extension_names)
		Print("\t%\n", CString(ext_name));

	VkDeviceCreateInfo device_create_info = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,

		.pQueueCreateInfos = &queue_create_info,
		.queueCreateInfoCount = 1,

		.pEnabledFeatures = &features,

		.ppEnabledExtensionNames = ldev_extension_names.elements,
		.enabledExtensionCount   = ldev_extension_names.count,

		.ppEnabledLayerNames = vk_enabled_layers,
		.enabledLayerCount   = vk_enabled_layer_count,
	};


	VkDevice dev;
	VkResult result = vkCreateDevice(pdev, &device_create_info, null, &dev);
	LogVar(ToString(result));
	Assert(result == VK_SUCCESS);

	ldev_extension_names.Free();

	return dev;
}

static Queue* CreateQueue(u32 family_index) {
	for (auto& queue : queues)
		if (queue.family == family_index)
			return &queue;

	VkQueue vk;
	vkGetDeviceQueue(device, family_index, family_index, &vk);

	Queue* queue = queues.Next();
	*queue = {
		.family = family_index,
		.vk = vk,
	};

	return queue;
}

static VkShaderModule LoadShader(String path) {
	VkShaderModule module;

	Print("Loading shader module: \"%\"...\n", path);

	Array<byte> code = LoadFile(path);
	Assert(code.length);
	Assert((code.length & 3) == 0);

	VkShaderModuleCreateInfo create_info = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pCode    = (u32*)code.data,
		.codeSize = code.length,
	};

	VkResult result = vkCreateShaderModule(device, &create_info, null, &module);
	Assert(result == VK_SUCCESS);

	code.Free();

	return module;
}

static void CreateRenderPass() {
	Print("Creating renderpass...\n");

	// Make this an array?
	VkAttachmentDescription color_attachment = {
		.format  = swapchain.surface_format.format,
		.samples = VK_SAMPLE_COUNT_1_BIT,

		.loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,

		.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,

		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout   = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
	};

	// Make this an array?
	VkAttachmentReference color_attachment_ref = {
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	};

	// Make this an array?
	VkSubpassDescription subpass = {
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,

		.pColorAttachments = &color_attachment_ref,
		.colorAttachmentCount = 1,
	};

	VkRenderPassCreateInfo renderpass_info = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,

		.pAttachments    = &color_attachment,
		.attachmentCount = 1,

		.pSubpasses   = &subpass,
		.subpassCount = 1,
	};

	VkResult vk_result = vkCreateRenderPass(device, &renderpass_info, null, &renderpass);
	Assert(vk_result == VK_SUCCESS);
}

static void CreatePipeline() {
	Print("Creating pipeline...\n");

	// Shaders
	VkPipelineShaderStageCreateInfo vert_stage_info = {
		.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.pName  = "main",
		.stage  = VK_SHADER_STAGE_VERTEX_BIT,
		.module = vert,
	};

	VkPipelineShaderStageCreateInfo frag_stage_info = {
		.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.pName  = "main",
		.stage  = VK_SHADER_STAGE_FRAGMENT_BIT,
		.module = frag,
	};

	VkPipelineShaderStageCreateInfo shader_stages[] = { vert_stage_info, frag_stage_info };
	u32 shader_stage_count = 2;

	// Dynamic State
	VkDynamicState dynamic_states[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR,
	};

	Assert(sizeof(dynamic_states) / sizeof(*dynamic_states) == 2);

	VkPipelineDynamicStateCreateInfo dynamic_state_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.pDynamicStates    = dynamic_states,
		.dynamicStateCount = sizeof(dynamic_states) / sizeof(*dynamic_states),
	};

	// Vertex Input
	VkPipelineVertexInputStateCreateInfo vertex_input_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,

		.pVertexBindingDescriptions    = null,
		.vertexBindingDescriptionCount = 0,

		.pVertexAttributeDescriptions    = null,
		.vertexAttributeDescriptionCount = 0,
	};

	// Input Assembly
	VkPipelineInputAssemblyStateCreateInfo input_assembly_state_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = false,
	};

	// Viewport
	VkViewport viewport = {
		.x = 0.0, .y = 0.0,
		.width  = (f32)swapchain.extent.width,
		.height = (f32)swapchain.extent.height,
		.minDepth = 0.0,
		.maxDepth = 1.0,
	};

	VkRect2D scissor = {
		.offset = { 0, 0 },
		.extent = swapchain.extent,
	};

	VkPipelineViewportStateCreateInfo viewport_state = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.scissorCount  = 1,
	};

	// Rasterizer
	VkPipelineRasterizationStateCreateInfo rasterizer_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable = false,
		.rasterizerDiscardEnable = false,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.lineWidth = 1.0,
		.cullMode = VK_CULL_MODE_BACK_BIT,
		.frontFace = VK_FRONT_FACE_CLOCKWISE,

		.depthBiasEnable         = false,
		.depthBiasConstantFactor = 0.0,
		.depthBiasClamp          = 0.0,
		.depthBiasSlopeFactor    = 0.0,

	};

	// Multisampling (not used)
	VkPipelineMultisampleStateCreateInfo multisampling_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.sampleShadingEnable = false,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.minSampleShading = 1.0,
		.pSampleMask = null,
		.alphaToCoverageEnable = false,
		.alphaToOneEnable = false,
	};

	// Color blending
	VkPipelineColorBlendAttachmentState color_blend_attachment_info = {
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,

		.blendEnable = true,

		.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
		.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		.colorBlendOp        = VK_BLEND_OP_ADD,

		.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		.alphaBlendOp        = VK_BLEND_OP_ADD,
	};

	VkPipelineColorBlendStateCreateInfo color_blend_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,

		.logicOpEnable = false, // @fixme Change this to true? logicOp needs to change?
		.logicOp = VK_LOGIC_OP_COPY,

		.pAttachments = &color_blend_attachment_info,
		.attachmentCount = 1,

		.blendConstants = { 0, 0, 0, 0 },
	};

	// Pipeline layout
	VkPipelineLayoutCreateInfo pipeline_layout_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,

		.pSetLayouts = null,
		.setLayoutCount = 0,

		.pPushConstantRanges = null,
		.pushConstantRangeCount = 0,
	};

	VkResult vk_result = vkCreatePipelineLayout(device, &pipeline_layout_info, null, &pipeline_layout);
	Assert(vk_result == VK_SUCCESS);

	VkGraphicsPipelineCreateInfo pipeline_info = {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,

		// Programmable stages
		.pStages    = shader_stages,
		.stageCount = shader_stage_count,

		// Fixed functions
		.pVertexInputState   = &vertex_input_info,
		.pInputAssemblyState = &input_assembly_state_info,
		.pViewportState      = &viewport_state,
		.pRasterizationState = &rasterizer_info,
		.pMultisampleState   = &multisampling_info,
		.pDepthStencilState  = null,
		.pColorBlendState    = &color_blend_info,
		.pDynamicState       = &dynamic_state_info,

		.layout = pipeline_layout,
		.renderPass = renderpass,
		.subpass = 0,

		// Derived pipeline (not used).
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex  = -1,
	};

	vk_result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_info, null, &pipeline);
	Assert(vk_result == VK_SUCCESS);
}

static void CreateCommandPool() {
	Print("Creating command pool...\n");

	VkCommandPoolCreateInfo command_pool_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, // Individual buffer recreation.
		.queueFamilyIndex = general_queue->family, // Graphics queue
	};

	VkResult vk_result = vkCreateCommandPool(device, &command_pool_info, null, &command_pool);
	Assert(vk_result == VK_SUCCESS);
}

int main(int argc, char** argv) {
	InitWindowSystem();

	window = CreateWindow();

	InitVulkan();
	window.InitSurface();

	physical_device = FindPhysicalDevice();
	queue_family_table = QueryQueueFamilyTable(physical_device);
	device = CreateLogicalDevice(physical_device);

	general_queue = CreateQueue(queue_family_table.graphics);

	swapchain = CreateSwapchain(physical_device, device, &window);

	vert = LoadShader("vert.spv");
	frag = LoadShader("frag.spv");

	CreateRenderPass();
	swapchain.InitFrameBuffers(device, renderpass);
	CreateCommandPool();

	CreatePipeline();

	while (!window.ShouldClose()) {
		window.Update();

		standard_output_buffer.Flush();
	}

	Print("Terminating...\n");
	vkDestroyCommandPool(device, command_pool, null);
	vkDestroyRenderPass(device, renderpass, null);
	vkDestroyPipelineLayout(device, pipeline_layout, null);
	vkDestroyShaderModule(device, vert, null);
	vkDestroyShaderModule(device, frag, null);
	swapchain.Destroy(device);
	window.Destroy();
	vkDestroyDevice(device, null);
	vkDestroyInstance(vk, null);
	glfwTerminate();

	standard_output_buffer.Flush();

	return 0;
}
