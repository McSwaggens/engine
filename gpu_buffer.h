#ifndef GPU_BUFFER_H
#define GPU_BUFFER_H

#include "general.h"
#include "assert.h"
#include "image.h"
#include <vulkan/vulkan.h>

struct GpuBuffer {
	u64 size = 0;
	void* mapping = null;

	VkBuffer buffer;
	VkBufferUsageFlags usage;

	VkDeviceMemory memory;
	VkMemoryPropertyFlags properties;

	void* Map();
	void  Unmap();

	void Upload(void* data, u64 size);

	void Upload(struct Image* image) {
		// Upload(image->data, image->GetSize());
	}

	void Destroy();

	void* GetMapping() {
		Assert(mapping);
		return mapping;
	}
};

static GpuBuffer CreateBuffer(u64 size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

static void CopyBuffer(GpuBuffer dst, GpuBuffer src, u64 size);
static void CopyBuffer(GpuBuffer dst, GpuBuffer src);

#endif // GPU_BUFFER_H
