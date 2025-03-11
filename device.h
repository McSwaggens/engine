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

	List<const char*> enabled_extensions;

	QueueFamilyTable queue_family_table;

	String name;

	Queue* general_queue;

	VkSemaphore CreateSemaphore();
	VkFence     CreateFence();

	VkCommandBuffer CreateCommandBuffer();

	void Init(VkPhysicalDevice pdev, QueueFamilyTable qft);
	void Destroy();

	Queue* CreateQueue(u32 family_index);

	void WaitIdle();
} static device = { };

#endif // DEVICE_H
