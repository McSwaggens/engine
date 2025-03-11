#include "device.h"

#include "fixed_allocator.h"
#include <vulkan/vulkan.h>
#include "vk_helper.h"

void Device::Init(VkPhysicalDevice pdev, QueueFamilyTable qft) {
	physical_device = pdev;
	queue_family_table = qft;

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

	enabled_extensions.Add(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	if (MACOS) enabled_extensions.Add("VK_KHR_portability_subset");

	Print("Device extensions:\n");
	for (const char* ext_name : enabled_extensions)
		Print("\t%\n", CString(ext_name));

	VkDeviceCreateInfo device_create_info = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,

		.pQueueCreateInfos = &queue_create_info,
		.queueCreateInfoCount = 1,

		.pEnabledFeatures = &features,

		.ppEnabledExtensionNames = enabled_extensions.elements,
		.enabledExtensionCount   = enabled_extensions.count,

		.ppEnabledLayerNames = vk_helper.enabled_layers.elements,
		.enabledLayerCount   = vk_helper.enabled_layers.count,
	};

	VkResult result = vkCreateDevice(physical_device, &device_create_info, null, &logical_device);
	LogVar(ToString(result));
	Assert(result == VK_SUCCESS);

	// @todo Set name
	general_queue = CreateQueue(queue_family_table.graphics);
}

static FixedAllocator<Queue, 32> queues;
Queue* Device::CreateQueue(u32 family_index) {
	for (auto& queue : queues)
		if (queue.family == family_index)
			return &queue;

	VkQueue vk;
	vkGetDeviceQueue(logical_device, family_index, family_index, &vk);

	Queue* queue = queues.Next();
	*queue = {
		.family = family_index,
		.vk = vk,
	};

	return queue;
}

void Device::Destroy() {
	vkDestroyDevice(logical_device, null);
}

void Device::WaitIdle() {
	vkDeviceWaitIdle(logical_device);
}

