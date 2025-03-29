#include "vk_helper.h"

#include "print.h"
#include "assert.h"

List<VkLayerProperties> QueryValidationLayers() {
	List<VkLayerProperties> layers;

	u32 layer_count;
	vkEnumerateInstanceLayerProperties(&layer_count, null);

	layers.AssureCount(layer_count);
	vkEnumerateInstanceLayerProperties(&layer_count, layers.elements);

	return layers;
}

static bool IsValidationLayerPresent(String str) {
	for (auto& layer : vk_helper.present_layers) {
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

static List<VkPhysicalDevice> QueryPhysicalDevices() {
	List<VkPhysicalDevice> result;
	u32 count = 0;
	vkEnumeratePhysicalDevices(vk_helper.instance, &count, null);
	result.AssureCount(count);
	vkEnumeratePhysicalDevices(vk_helper.instance, &count, result.elements);
	result.count = count;
	return result;
}

static VKAPI_ATTR VkBool32 ValidationLayerDebugMessageCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT severity,
	VkDebugUtilsMessageTypeFlagsEXT type,
	const VkDebugUtilsMessengerCallbackDataEXT* msg,
	void* user_data) {

	Print("%\n", CString(msg->pMessage));
	return false;
}

void VkHelper::Init() {
	enabled_layers.Add("VK_LAYER_KHRONOS_validation");

	List<const char*> glfw_required_extensions = QueryGlfwRequiredExtensions();

	enabled_extensions.Add(glfw_required_extensions);
	enabled_extensions.Add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	if (MACOS) enabled_extensions.Add(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

	VkApplicationInfo app_info = {
		.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName   = "Engine",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName        = "xxx",
		.engineVersion      = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion         = VK_API_VERSION_1_4,
	};

	present_layers = QueryValidationLayers();

	for (auto layer_name : enabled_layers)
		Assert(IsValidationLayerPresent(CString(layer_name)));

	VkInstanceCreateInfo inst_info = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		#if MACOS
			.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
		#endif
		.pApplicationInfo = &app_info,

		.ppEnabledExtensionNames = enabled_extensions.elements,
		.enabledExtensionCount   = enabled_extensions.count,

		.ppEnabledLayerNames = enabled_layers.elements,
		.enabledLayerCount   = enabled_layers.count,
	};

	VkResult result = vkCreateInstance(&inst_info, null, &instance);
	Assert(result == VK_SUCCESS);

	InitDebugMessageCallback();
	physical_devices = QueryPhysicalDevices();
}

void VkHelper::InitDebugMessageCallback() {
	VkDebugUtilsMessengerCreateInfoEXT info = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,

		.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT    |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,

		.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT  |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT,

		.pUserData = null,
		.pfnUserCallback = ValidationLayerDebugMessageCallback,
	};

	PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	vkCreateDebugUtilsMessengerEXT(instance, &info, null, &debug_messenger);
}

static bool IsPhysicalDeviceGood(VkPhysicalDevice pdev, Window* window) {
	VkPhysicalDeviceProperties props;
	vkGetPhysicalDeviceProperties(pdev, &props);

	// @Todo: Score devices and pick the one with the highest score.
	// Also need to make sure the device selected has the nessesary features present.
	// This is good enough for now.

	if (props.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
		props.deviceType != VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
		return false;

	QueueFamilyTable queue_table = QueryQueueFamilyTable(pdev, window);
	if (!queue_table.IsComplete())
		return false;

	SwapchainSupportInfo swapchain_info = QuerySwapchainSupportInfo(pdev, window->surface);
	if (!swapchain_info.formats.count || !swapchain_info.present_modes.count) {
		swapchain_info.Free();
		return false;
	}

	swapchain_info.Free();
	return true;
}

VkPhysicalDevice VkHelper::FindPhysicalDevice(Window* window) {
	for (VkPhysicalDevice pdev : vk_helper.physical_devices)
		if (IsPhysicalDeviceGood(pdev, window))
			return pdev;

	Assert(false);
	return VK_NULL_HANDLE;
}

void VkHelper::Destroy() {
	vkDestroyInstance(instance, null);
}
