#ifndef QUEUE_H
#define QUEUE_H

#include "general.h"
#include "window.h"
#include "command_buffer.h"

#include <vulkan/vulkan.h>

struct Queue {
	VkQueue vk;
	u32 index;
	u32 family;

	void Wait() {
		vkQueueWaitIdle(vk);
	}

	void ExecuteAsync(CommandBuffer cmdbuf, VkFence fence = VK_NULL_HANDLE) {
		VkSubmitInfo submit_info = {
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.commandBufferCount = 1,
			.pCommandBuffers = &cmdbuf.handle,
		};

		vkQueueSubmit(vk, 1, &submit_info, fence);
	}

	void Execute(CommandBuffer cmdbuf) {
		ExecuteAsync(cmdbuf, VK_NULL_HANDLE);
		Wait();
	}
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
