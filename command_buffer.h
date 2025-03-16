#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include "general.h"

#include <vulkan/vulkan.h>

struct CommandBuffer {
	VkCommandBuffer handle;
	bool recording;

	void Begin(bool singleshot = false) {
		VkCommandBufferBeginInfo info {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = 0,
			.pInheritanceInfo = null,
		};

		if (singleshot)
			info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(handle, &info);
		recording = true;
	}
	void End()   {
		vkEndCommandBuffer(handle);
		recording = false;
	}

	void Reset() {
		vkResetCommandBuffer(handle, 0);
		recording = true;
	}

	void Transfer(VkBuffer dst, VkBuffer src, u64 size) {
		Assert(recording);

		VkBufferCopy region = {
			.dstOffset = 0,
			.srcOffset = 0,
			.size = size,
		};

		vkCmdCopyBuffer(handle, src, dst, 1, &region);
	}

	void Destroy() {
		// vkDestroyBuffer(device.logical_device, handle, null);
	}
};

#endif // COMMAND_BUFFER_H
