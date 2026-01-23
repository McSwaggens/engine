
#include "command_buffer.h"
#include "device.h"

void CommandBuffer::Destroy() {
	vkFreeCommandBuffers(device.logical_device, device.command_pool, 1, &handle);
}
