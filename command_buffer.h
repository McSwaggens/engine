#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include "general.h"
#include "assert.h"
#include "vk_helper.h"
#include "vector.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

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

	void End() {
		Assert(recording);
		vkEndCommandBuffer(handle);
		recording = false;
	}

	void Reset() {
		Assert(!recording);
		vkResetCommandBuffer(handle, 0);
		recording = false;
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

	void BeginRenderPass(VkRenderPassBeginInfo* info, VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE) {
		Assert(recording);
		vkCmdBeginRenderPass(handle, info, contents);
	}

	void EndRenderPass() {
		Assert(recording);
		vkCmdEndRenderPass(handle);
	}

	void BindPipeline(VkPipelineBindPoint bind_point, VkPipeline pipeline) {
		Assert(recording);
		vkCmdBindPipeline(handle, bind_point, pipeline);
	}

	void SetViewport(Vector2 offset, Vector2 size, Vector2 depth) {
		Assert(recording);
		VkViewport viewport {
			.x = offset.x, .y = offset.y,
			.width = size.x, .height = size.y,
			.minDepth = depth.x, .maxDepth = depth.y,
		};
		vkCmdSetViewport(handle, 0, 1, &viewport);
	}

	void SetScissor(Vector2 offset, Vector2 size) {
		Assert(recording);
		VkRect2D scissor {
			.offset = { (s32)offset.x, (s32)offset.y },
			.extent = { (u32)size.x, (u32)size.y },
		};
		vkCmdSetScissor(handle, 0, 1, &scissor);
	}

	void BindVertexBuffers(VkBuffer* buffers, VkDeviceSize* offsets, u32 count = 1) {
		Assert(recording);
		vkCmdBindVertexBuffers(handle, 0, count, buffers, offsets);
	}

	void BindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType index_type) {
		Assert(recording);
		vkCmdBindIndexBuffer(handle, buffer, offset, index_type);
	}

	void BindDescriptorSets(VkPipelineBindPoint bind_point, VkPipelineLayout layout, u32 first_set, u32 count, VkDescriptorSet* sets) {
		Assert(recording);
		vkCmdBindDescriptorSets(handle, bind_point, layout, first_set, count, sets, 0, null);
	}

	void DrawIndexed(u32 index_count, u32 instance_count, u32 first_index, s32 vertex_offset, u32 first_instance) {
		Assert(recording);
		vkCmdDrawIndexed(handle, index_count, instance_count, first_index, vertex_offset, first_instance);
	}

	void Destroy();
};

#endif // COMMAND_BUFFER_H
