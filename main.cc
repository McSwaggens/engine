#include "general.h"
#include "math.h"
#include "window.h"

#include "vector.h"
#include "quaternion.h"

#include "list.h"

#include "assert.cc"
#include "alloc.cc"
#include "unix.cc"
#include "window.cc"
#include "print.cc"
#include "file_system.cc"

#include "vk_helper.h"

static VkInstance vk;
static List<VkLayerProperties> vk_layers;
static List<VkPhysicalDevice> physical_devices;

static const char* vk_enabled_layers[] = {
	"VK_LAYER_KHRONOS_validation",
};

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
	List<const char*> required_extensions;
	List<const char*> glfw_required_extensions = QueryGlfwRequiredExtensions();

	required_extensions.Add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	// required_extensions.Add(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

	Print("Required Extensions:\n");
	for (auto ext : required_extensions)
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
		// .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
		.pApplicationInfo = &app_info,

		.ppEnabledExtensionNames = required_extensions.elements,
		.enabledExtensionCount   = required_extensions.count,

		.ppEnabledLayerNames = vk_enabled_layers,
		.enabledLayerCount   = 1,
	};

	VkResult result = vkCreateInstance(&inst_info, null, &vk);
	Print("result = %\n", ToString(result));
	Assert(result == VK_SUCCESS);

	required_extensions.Free();
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

static bool IsPhysicalDeviceGood(VkPhysicalDevice pdev) {
	VkPhysicalDeviceProperties props;
	vkGetPhysicalDeviceProperties(pdev, &props);

	// @Todo: Score devices and pick the one with the highest score.
	// Also need to make sure the device selected has the nessesary features present.
	// This is good enough for now.

	if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU || props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
		Print("Using Physical Device: %\n", CString(props.deviceName));
		return true;
	}

	return false;
}

static VkPhysicalDevice FindPhysicalDevice() {
	physical_devices = QueryPhysicalDevices();

	for (VkPhysicalDevice pdev : physical_devices)
		if (IsPhysicalDeviceGood(pdev))
			return pdev;

	Assert(false);
	return null;
}

int main(int argc, char** argv) {
	InitWindowSystem();

	Window window = CreateWindow();

	InitVulkan();
	FindPhysicalDevice();

	while (!window.ShouldClose()) {
		window.Update();

		standard_output_buffer.Flush();
	}

	vkDestroyInstance(vk, null);
	window.Destroy();
	glfwTerminate();

	standard_output_buffer.Flush();

	return 0;
}
