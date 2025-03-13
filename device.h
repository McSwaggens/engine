#ifndef DEVICE_H
#define DEVICE_H

#include "general.h"
#include "string.h"
#include "list.h"
#include "queue.h"

#include <vulkan/vulkan.h>

struct Device {
	VkPhysicalDevice physical_device;
	VkDevice         logical_device;
	VkCommandPool    command_pool;
	VkPhysicalDeviceProperties       physical_properties;
	VkPhysicalDeviceMemoryProperties memory_properties;

	List<const char*> enabled_extensions;

	QueueFamilyTable queue_family_table;

	String name;

	Queue* general_queue;

	void Init(VkPhysicalDevice pdev, QueueFamilyTable qft);
	void InitCommandPool();

	void Destroy();

	VkSemaphore CreateSemaphore();
	VkFence     CreateFence(bool signalled = false);

	VkCommandBuffer CreateCommandBuffer();
	void CreateCommandBuffers(VkCommandBuffer* out_command_buffers, u32 count);

	Queue* CreateQueue(u32 family_index);

	u32 FindMemoryType(u32 filter, VkMemoryPropertyFlags properties);
	VkDeviceMemory AllocateMemory(u64 size, u32 type_index);
	// VkDeviceMemory AllocateMemory(u64 size, VkMemoryRequirements memory_requirements);

	void WaitIdle();
};

static Device device = { };

#endif // DEVICE_H
