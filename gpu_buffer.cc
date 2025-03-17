#include "gpu_buffer.h"
#include "device.h"

static GpuBuffer CreateBuffer(u64 size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
	GpuBuffer result = {
		.size = size,
	};

	VkBufferCreateInfo buffer_info = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.usage = usage,
		.size = size,
	};

	VkResult vk_result = vkCreateBuffer(device.logical_device, &buffer_info, null, &result.buffer);
	Assert(vk_result == VK_SUCCESS);

	VkMemoryRequirements memreq;
	vkGetBufferMemoryRequirements(device.logical_device, result.buffer, &memreq);

	result.memory = device.AllocateMemory(memreq.size, device.FindMemoryType(memreq.memoryTypeBits, properties));

	vkBindBufferMemory(device.logical_device, result.buffer, result.memory, 0);

	return result;
}

static void CopyBuffer(GpuBuffer dst, GpuBuffer src) {
	Assert(dst.size >= src.size);
	CopyBuffer(dst, src, src.size);
}

static void CopyBuffer(GpuBuffer dst, GpuBuffer src, u64 size) {
	Assert(dst.size >= size);

	CommandBuffer proc = device.CreateCommandBuffer();
	proc.Begin(true);
	proc.Transfer(dst.buffer, src.buffer, size);
	proc.End();

	device.general_queue->Execute(proc);

	proc.Destroy();
}

void* GpuBuffer::Map() {
	if (!mapping)
		vkMapMemory(device.logical_device, memory, 0, size, 0, &mapping);

	return mapping;
}

void GpuBuffer::Unmap() {
	Assert(mapping);

	vkUnmapMemory(device.logical_device, memory);
	mapping = null;
}


void GpuBuffer::Upload(void* data, u64 size) {
	Assert(this->size >= size);

	bool already_mapped = mapping;

	if (!already_mapped)
		Map();

	CopyMemory(mapping, data, size);

	if (!already_mapped)
		Unmap();
}

void GpuBuffer::Destroy() {
	vkDestroyBuffer(device.logical_device, buffer, null);
	vkFreeMemory(device.logical_device, memory, null);

	SetMemory(this, 0, sizeof(*this));
}

