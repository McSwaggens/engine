#include "queue.h"

static bool CanQueueFamilyPresent(VkPhysicalDevice pdev, VkSurfaceKHR surface, u32 family) {
	VkBool32 can_present = false;
	vkGetPhysicalDeviceSurfaceSupportKHR(pdev, family, surface, &can_present);
	return can_present;
}

static QueueFamilyTable QueryQueueFamilyTable(VkPhysicalDevice pdev, Window* window) {
	u32 count;
	vkGetPhysicalDeviceQueueFamilyProperties(pdev, &count, null);

	VkQueueFamilyProperties queue_props[count];
	vkGetPhysicalDeviceQueueFamilyProperties(pdev, &count, queue_props);

	QueueFamilyTable result;

	u32 all = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
	for (u32 i = 0; i < count; i++) {
		VkQueueFamilyProperties* props = &queue_props[i];

		u32 feature_count = PopCount(props->queueFlags);

		if ((props->queueFlags & VK_QUEUE_GRAPHICS_BIT) && result.graphics == -1)
			result.graphics = i;

		if ((props->queueFlags & VK_QUEUE_COMPUTE_BIT)  && result.compute  == -1)
			result.compute = i;

		if ((props->queueFlags & VK_QUEUE_TRANSFER_BIT) && result.transfer == -1)
			result.transfer = i;

		bool can_present = CanQueueFamilyPresent(pdev, window->surface, i);

		if (can_present && result.present == -1)
			result.present = i;
	}

	return result;
}

