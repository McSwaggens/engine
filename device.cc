#include "device.h"

#include "fixed_allocator.h"
#include <vulkan/vulkan.h>
#include "vk_helper.h"

void Device::Init(VkPhysicalDevice pdev, QueueFamilyTable qft) {
	physical_device = pdev;
	queue_family_table = qft;

	vkGetPhysicalDeviceProperties(physical_device, &physical_properties);
	vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);
	name = CString(physical_properties.deviceName);

	Print("Using graphics card: %\n", name);

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
	Assert(result == VK_SUCCESS);

	general_queue = CreateQueue(queue_family_table.graphics);
	InitCommandPool();
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

VkSemaphore Device::CreateSemaphore() {
	VkSemaphore result = VK_NULL_HANDLE;

	VkSemaphoreCreateInfo semaphore_info = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.flags = 0,
	};

	VkResult vk_result = vkCreateSemaphore(logical_device, &semaphore_info, null, &result);
	Assert(vk_result == VK_SUCCESS);

	standard_output_buffer.Flush();
 	return result;
}

VkFence Device::CreateFence(bool signalled) {
	VkFence result;

	VkFenceCreateInfo fence_info = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.flags = 0,
	};

	if (signalled)
		fence_info.flags |= VK_FENCE_CREATE_SIGNALED_BIT;

	VkResult vk_result = vkCreateFence(logical_device, &fence_info, null, &result);
	Assert(vk_result == VK_SUCCESS);

	return result;
}

void Device::InitCommandPool() {
	VkCommandPoolCreateInfo command_pool_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, // Individual buffer recreation.
		.queueFamilyIndex = device.general_queue->family, // Graphics queue
	};

	VkResult vk_result = vkCreateCommandPool(logical_device, &command_pool_info, null, &command_pool);
	Assert(vk_result == VK_SUCCESS);
}

CommandBuffer Device::CreateCommandBuffer() {
	VkCommandBuffer cmdbuf;
	CreateCommandBuffers(&cmdbuf, 1);

	CommandBuffer result = {
		.handle = cmdbuf,
	};

	return result;
}

void Device::CreateCommandBuffers(VkCommandBuffer* out_command_buffers, u32 count) {
	Assert(count > 0);

	VkCommandBufferAllocateInfo alloc_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = command_pool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = count,
	};

	VkResult vk_result = vkAllocateCommandBuffers(logical_device, &alloc_info, out_command_buffers);
	Assert(vk_result == VK_SUCCESS);
}

u32 Device::FindMemoryType(u32 filter, VkMemoryPropertyFlags properties) {
	for (u32 i = 0; i < memory_properties.memoryTypeCount; i++) {
		VkMemoryType type = memory_properties.memoryTypes[i];

		if (!(filter & (1 << i)))
			continue;

		if ((type.propertyFlags & properties) != properties)
			continue;

		return i;
	}

	Assert(false);
	return -1;
}

VkDeviceMemory Device::AllocateMemory(u64 size, u32 type_index) {
	VkDeviceMemory device_memory;

	VkMemoryAllocateInfo alloc_info = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = size,
		.memoryTypeIndex = type_index,
	};

	VkResult vk_result = vkAllocateMemory(logical_device, &alloc_info, null, &device_memory);
	Assert(vk_result == VK_SUCCESS);

	return device_memory;
}

void Device::WaitIdle() {
	vkDeviceWaitIdle(logical_device);
}

void Device::Destroy() {
	vkDestroyCommandPool(logical_device, command_pool, null);
	vkDestroyDevice(logical_device, null);
}

