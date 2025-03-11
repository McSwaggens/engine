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
	VkPhysicalDeviceProperties physical_properties;

	List<const char*> enabled_extensions;

	QueueFamilyTable queue_family_table;

	String name;

	Queue* general_queue;

	VkSemaphore CreateSemaphore();
	VkFence     CreateFence(bool signalled = false);

	VkCommandBuffer CreateCommandBuffer();

	void Init(VkPhysicalDevice pdev, QueueFamilyTable qft);
	void InitCommandPool();

	void Destroy();

	Queue* CreateQueue(u32 family_index);

	void WaitIdle();
} static device = { };

#endif // DEVICE_H
