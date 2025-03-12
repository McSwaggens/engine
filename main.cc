#include "general.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "math.h"
#include "window.h"
#include "os.h"

#include "assert.cc"
#include "alloc.cc"
#include "unix.cc"
#include "window.cc"
#include "print.cc"
#include "file_system.cc"
#include "swapchain.cc"
#include "device.cc"
#include "queue.cc"
#include "vk_helper.cc"

#include "vk_helper.h"
#include "vector.h"
#include "quaternion.h"
#include "list.h"
#include "swapchain.h"
#include "fixed_allocator.h"
#include "device.h"

static Window window;
static Swapchain swapchain;
static VkShaderModule vert;
static VkShaderModule frag;
static VkRenderPass renderpass;
static VkPipelineLayout pipeline_layout;
static VkPipeline pipeline;

static const u32 INFLIGHT_FRAME_COUNT = 2;

static List<VkCommandBuffer> command_buffers;
static List<VkSemaphore>     image_available_semaphores;
static List<VkSemaphore>     render_finished_semaphores;
static List<VkFence>         inflight_fences;

static u64 frame_counter = 0;
static u64 inflight_frame_index = 0;
static f64 time          = 0.0;
static u64 time_us       = 0;
static u64 init_time_us  = 0;
static u64 fps = 0;

static void InitTime() {
	init_time_us = GetTimeMicroseconds();
	time_us = init_time_us;
}

static void UpdateTime() {
	time_us = GetTimeMicroseconds();
	time = (time_us - init_time_us) / 1'000'000.0;
}

static VkShaderModule LoadShader(String path) {
	VkShaderModule module;

	Array<byte> code = LoadFile(path);
	Assert(code.length);
	Assert((code.length & 3) == 0);

	VkShaderModuleCreateInfo create_info = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pCode    = (u32*)code.data,
		.codeSize = code.length,
	};

	VkResult result = vkCreateShaderModule(device.logical_device, &create_info, null, &module);
	Assert(result == VK_SUCCESS);

	code.Free();

	return module;
}

static void CreateRenderPass() {
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

	VkSubpassDependency dependency = {
		.srcSubpass = VK_SUBPASS_EXTERNAL, // Implicit subpass
		.dstSubpass = 0, // Our subpass index.

		.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // Wait for swapchain to finish reading colors.
		.srcAccessMask = 0,

		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
	};

	VkRenderPassCreateInfo renderpass_info = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,

		.pAttachments    = &color_attachment,
		.attachmentCount = 1,

		.pSubpasses   = &subpass,
		.subpassCount = 1,

		.dependencyCount = 1,
		.pDependencies   = &dependency,
	};

	VkResult vk_result = vkCreateRenderPass(device.logical_device, &renderpass_info, null, &renderpass);
	Assert(vk_result == VK_SUCCESS);
}

static void CreatePipeline() {
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

	VkResult vk_result = vkCreatePipelineLayout(device.logical_device, &pipeline_layout_info, null, &pipeline_layout);
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

	vk_result = vkCreateGraphicsPipelines(device.logical_device, VK_NULL_HANDLE, 1, &pipeline_info, null, &pipeline);
	Assert(vk_result == VK_SUCCESS);
}

static void RecordCommandBuffer(VkCommandBuffer command_buffer, u32 image_index) {
	VkCommandBufferBeginInfo begin_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = 0,
		.pInheritanceInfo = null,
	};

	VkResult vk_result = vkBeginCommandBuffer(command_buffer, &begin_info);
	Assert(vk_result == VK_SUCCESS);

	VkClearValue clear_color = {
		.color = { .float32 = { 0.0, 0.0, 0.0, 1.0 } },
	};

	VkRenderPassBeginInfo renderpass_begin_info = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass = renderpass,
		.framebuffer = swapchain.framebuffers[image_index],

		.renderArea = {
			.offset = { 0, 0 },
			.extent = swapchain.extent,
		},

		.pClearValues    = &clear_color,
		.clearValueCount = 1,
	};

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

	vkCmdBeginRenderPass(command_buffer, &renderpass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	vkCmdSetViewport(command_buffer, 0, 1, &viewport);
	vkCmdSetScissor(command_buffer,  0, 1, &scissor);
	vkCmdDraw(command_buffer, 3, 1, 0, 0);
	vkCmdEndRenderPass(command_buffer);

	vk_result = vkEndCommandBuffer(command_buffer);
	Assert(vk_result == VK_SUCCESS);
}

static void DrawFrame() {
	vkWaitForFences(device.logical_device, 1, &inflight_fences[inflight_frame_index], true, -1);
	vkResetFences(device.logical_device,   1, &inflight_fences[inflight_frame_index]);

	u32 image = swapchain.GetNextImageIndex(image_available_semaphores[inflight_frame_index]);

	vkResetCommandBuffer(command_buffers[inflight_frame_index], 0);
	RecordCommandBuffer(command_buffers[inflight_frame_index], image);

	// Wait for an image before rendering colors.
	VkSemaphore          wait_semaphores[] = { image_available_semaphores[inflight_frame_index] };
	VkPipelineStageFlags wait_stages[]     = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT    };

	// Semaphores to signal when we're done rendering the frame.
	VkSemaphore signal_semaphores[] = { render_finished_semaphores[inflight_frame_index] };

	VkSubmitInfo submit_info = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = wait_semaphores,
		.pWaitDstStageMask = wait_stages,

		.pCommandBuffers = &command_buffers[inflight_frame_index],
		.commandBufferCount = 1,

		.signalSemaphoreCount = 1,
		.pSignalSemaphores = signal_semaphores,
	};

	VkResult vk_result = vkQueueSubmit(device.general_queue->vk, 1, &submit_info, inflight_fences[inflight_frame_index]);
	Assert(vk_result == VK_SUCCESS);

	VkPresentInfoKHR present_info = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = signal_semaphores,
		.swapchainCount = 1,
		.pSwapchains = &swapchain.handle,
		.pImageIndices = &image,
		.pResults = null,
	};

	vk_result = vkQueuePresentKHR(device.general_queue->vk, &present_info);
}

int main(int argc, char** argv) {
	Print("Initializing...\n");

	InitGlobalAllocator();

	InitTime();
	InitWindowSystem();
	window = CreateWindow();
	vk_helper.Init();
	window.InitSurface();

	VkPhysicalDevice physical_device = vk_helper.FindPhysicalDevice(&window);
	QueueFamilyTable qft = QueryQueueFamilyTable(physical_device, &window);

	device.Init(physical_device, qft);
	swapchain = CreateSwapchain(&window);

	vert = LoadShader("vert.spv");
	frag = LoadShader("frag.spv");

	CreateRenderPass();
	CreatePipeline();

	swapchain.InitFrameBuffers(renderpass);

	command_buffers.AssureCount(INFLIGHT_FRAME_COUNT);
	image_available_semaphores.AssureCount(INFLIGHT_FRAME_COUNT);
	render_finished_semaphores.AssureCount(INFLIGHT_FRAME_COUNT);
	inflight_fences.AssureCount(INFLIGHT_FRAME_COUNT);

	device.CreateCommandBuffers(command_buffers.elements, INFLIGHT_FRAME_COUNT);
	for (u32 i = 0; i < INFLIGHT_FRAME_COUNT; i++) {
		image_available_semaphores[i] = device.CreateSemaphore();
		render_finished_semaphores[i] = device.CreateSemaphore();
		inflight_fences[i] = device.CreateFence(true);
	}

	Print("Running...\n");

	UpdateTime();

	f64 last_frame_time = time;
	f64 last_second_time = time;
	u64 last_second_frame_counter = 0;

	while (!window.ShouldClose()) {
		UpdateTime();
		window.Update();

		inflight_frame_index = frame_counter % INFLIGHT_FRAME_COUNT;

		if (time - last_second_time >= 1.0) {
			last_second_time = time;

			fps = frame_counter - last_second_frame_counter;
			last_second_frame_counter = frame_counter;

			LogVar(fps);
		}

		DrawFrame();

		frame_counter++;
		last_frame_time = time;
		standard_output_buffer.Flush();
	}

	device.WaitIdle();

	Print("Terminating...\n");
	for (VkSemaphore semaphore : image_available_semaphores) vkDestroySemaphore(device.logical_device, semaphore, null);
	for (VkSemaphore semaphore : render_finished_semaphores) vkDestroySemaphore(device.logical_device, semaphore, null);

	for (VkFence fence : inflight_fences) vkDestroyFence(device.logical_device, fence, null);

	vkDestroyRenderPass(device.logical_device, renderpass, null);
	vkDestroyPipelineLayout(device.logical_device, pipeline_layout, null);
	vkDestroyPipeline(device.logical_device, pipeline, null);

	vkDestroyShaderModule(device.logical_device, vert, null);
	vkDestroyShaderModule(device.logical_device, frag, null);

	swapchain.Destroy();
	window.Destroy();
	device.Destroy();
	vk_helper.Destroy();
	glfwTerminate();

	Print("Goodbye!\n");
	standard_output_buffer.Flush();

	return 0;
}
