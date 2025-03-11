#ifndef QUEUE_H
#define QUEUE_H

#include "general.h"
#include <vulkan/vulkan.h>

struct Queue {
	VkQueue vk;
	u32 index;
	u32 family;
};

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

static QueueFamilyTable QueryQueueFamilyTable(VkPhysicalDevice pdev, Window* window);

#endif // QUEUE_H
