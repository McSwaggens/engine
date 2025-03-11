#include "vk_helper.h"

#include "print.h"
#include "assert.h"

List<VkLayerProperties> QueryValidationLayers() {
	List<VkLayerProperties> layers;

	u32 layer_count;
	vkEnumerateInstanceLayerProperties(&layer_count, null);

	layers.AssureCount(layer_count);
	vkEnumerateInstanceLayerProperties(&layer_count, layers.elements);

	Print("Layers:\n");
	for (auto layer : layers)
		Print("\t%\n", CString(layer.layerName));

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
		#ifdef MACOS
			.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
		#endif
		.pApplicationInfo = &app_info,

		.ppEnabledExtensionNames = enabled_extensions.elements,
		.enabledExtensionCount   = enabled_extensions.count,

		.ppEnabledLayerNames = enabled_layers.elements,
		.enabledLayerCount   = enabled_layers.count,
	};

	VkResult result = vkCreateInstance(&inst_info, null, &instance);
	LogVar(ToString(result));
	Assert(result == VK_SUCCESS);

	physical_devices = QueryPhysicalDevices();
}

