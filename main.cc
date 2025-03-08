#include "general.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
static VkInstance vk;

#include "math.h"
#include "window.h"

#define LogVar(var) Print(#var " = %\n", var)

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

struct Queue {
	VkQueue vk;
	u32 index;
	u32 family;

	// bool CanPresent(VkPhysicalDevice pdev, VkSurfaceKHR surface) {
	// }
};

template<typename T, u32 N>
struct FixedAllocator {
	T stack[N];
	u32 head;

	T* Next() {
		Assert(head < N);
		return &stack[head++];
	}

	T* begin() { return stack; }
	T* end()   { return stack + head; }
};

static FixedAllocator<Queue, 32> queues;
static Queue* general_queue;

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

static List<VkLayerProperties> vk_layers;
static List<VkPhysicalDevice> physical_devices;
static VkPhysicalDevice physical_device;
static VkDevice device;
static QueueFamilyTable queue_family_table;
static List<const char*> vk_enabled_extensions;

static VkSurfaceKHR surface;
static Window window;

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

	Print("Using Physical Device: %\n", CString(props.deviceName));
	return true;
}

static VkPhysicalDevice FindPhysicalDevice() {
	physical_devices = QueryPhysicalDevices();

	for (VkPhysicalDevice pdev : physical_devices)
		if (IsPhysicalDeviceGood(pdev))
			return pdev;

	Assert(false);
	return null;
}

static VkDevice CreateLogicalDevice(VkPhysicalDevice pdev) {
	float priority = 1.0;
	standard_output_buffer.Flush();
	VkDeviceQueueCreateInfo queue_create_info = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = queue_family_table.graphics,
		.queueCount = 1,
		.pQueuePriorities = &priority,
	};

	VkPhysicalDeviceFeatures features = { };

	Print("Device extensions:\n");
	for (const char* ext_name : vk_enabled_extensions) {
		Print("\t%\n", CString(ext_name));
		standard_output_buffer.Flush();
	}

	List<const char*> ldev_extension_names;
	ldev_extension_names.Add("VK_KHR_portability_subset");

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

static Queue* CreateQueue(VkDevice device, u32 family_index) {
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

int main(int argc, char** argv) {
	InitWindowSystem();

	window = CreateWindow();

	InitVulkan();
	window.InitSurface();
	LogVar((u64)window.surface);

	physical_device = FindPhysicalDevice();
	queue_family_table = QueryQueueFamilyTable(physical_device);
	device = CreateLogicalDevice(physical_device);
	general_queue = CreateQueue(device, queue_family_table.graphics);

	while (!window.ShouldClose()) {
		window.Update();

		standard_output_buffer.Flush();
	}

	Print("Terminating...\n");
	window.Destroy();
	vkDestroyDevice(device, null);
	vkDestroyInstance(vk, null);
	glfwTerminate();

	standard_output_buffer.Flush();

	return 0;
}
