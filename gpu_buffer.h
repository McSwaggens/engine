#ifndef GPU_BUFFER_H
#define GPU_BUFFER_H

#include "general.h"
#include <vulkan/vulkan.h>

struct GpuBuffer {
	u64 size;

	VkBuffer buffer;
	VkBufferUsageFlags usage;

	VkDeviceMemory memory;
	VkMemoryPropertyFlags properties;

	void Upload(void* data, u64 size);
	void Destroy();
};

static GpuBuffer CreateBuffer(u64 size,  VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

static void CopyBuffer(GpuBuffer dst, GpuBuffer src, u64 size);
static void CopyBuffer(GpuBuffer dst, GpuBuffer src);

#endif // GPU_BUFFER_H
