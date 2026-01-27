#include "swapchain.h"

#include "vk_helper.h"
#include "print.h"

#include "device.h"

static VkFormat FindDepthFormat() {
	VkFormat formats[] = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
	for (VkFormat fmt : formats) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(device.physical_device, fmt, &props);
		if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
			return fmt;
	}
	Assert(false);
	return VK_FORMAT_UNDEFINED;
}

void Swapchain::Init(Window* window) {
	SwapchainSupportInfo swapchain_info = QuerySwapchainSupportInfo(device.physical_device, window->surface);
	VkPresentModeKHR present_mode = swapchain_info.ChoosePresentMode();
	surface_format = swapchain_info.ChooseFormat();
	extent = swapchain_info.GetExtent(window);
	u32 image_count = swapchain_info.GetImageCount();

	VkSwapchainCreateInfoKHR swapchain_create_info = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,

		.surface          = window->surface,
		.minImageCount    = image_count,
		.imageFormat      = surface_format.format,
		.imageColorSpace  = surface_format.colorSpace,
		.imageExtent      = extent,
		.imageArrayLayers = 1,
		.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,

		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE, // present queue and graphics queue are the same.
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices   = null,

		.preTransform = swapchain_info.capabilities.currentTransform, // No transform.
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = present_mode,
		.clipped = true,
		.oldSwapchain = null,
	};

	VkResult result = vkCreateSwapchainKHR(device.logical_device, &swapchain_create_info, null, &handle);
	Assert(result == VK_SUCCESS);

	InitImages();
	InitViews();

	// Create depth buffer
	depth_format = FindDepthFormat();

	VkImageCreateInfo depth_image_info = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = depth_format,
		.extent = { extent.width, extent.height, 1 },
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
	};
	vkCreateImage(device.logical_device, &depth_image_info, null, &depth_image);

	VkMemoryRequirements memreq;
	vkGetImageMemoryRequirements(device.logical_device, depth_image, &memreq);
	depth_memory = device.AllocateMemory(memreq.size, device.FindMemoryType(memreq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
	vkBindImageMemory(device.logical_device, depth_image, depth_memory, 0);

	VkImageViewCreateInfo depth_view_info = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = depth_image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = depth_format,
		.subresourceRange = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 },
	};
	vkCreateImageView(device.logical_device, &depth_view_info, null, &depth_view);

	swapchain_info.Free();
}

void Swapchain::InitImages() {
	images.Reset();

	u32 image_count;
	vkGetSwapchainImagesKHR(device.logical_device, handle, &image_count, null);

	images.AssureCount(image_count);
	vkGetSwapchainImagesKHR(device.logical_device, handle, &image_count, images.elements);
}

void Swapchain::InitViews() {
	views.Reset();
	views.AssureCount(images.count);

	for (u32 i = 0; i < images.count; i++) {
		VkImage* image = &images[i];
		VkImageViewCreateInfo image_view_create_info = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,

			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.image = *image,
			.format = surface_format.format,

			.components = {
				.r = VK_COMPONENT_SWIZZLE_IDENTITY,
				.g = VK_COMPONENT_SWIZZLE_IDENTITY,
				.b = VK_COMPONENT_SWIZZLE_IDENTITY,
				.a = VK_COMPONENT_SWIZZLE_IDENTITY,
			},

			.subresourceRange = {
				.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel   = 0,
				.levelCount     = 1,
				.baseArrayLayer = 0,
				.layerCount     = 1,
			},
		};

		vkCreateImageView(device.logical_device, &image_view_create_info, null, &views[i]);
	}
}

void Swapchain::InitFrameBuffers(VkRenderPass renderpass) {
	framebuffers.AssureCount(images.count);

	for (u32 i = 0; i < images.count; i++) {
		VkImageView attachments[] = {
			views[i],
			depth_view,
		};

		VkFramebufferCreateInfo framebuffer_info = {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.renderPass = renderpass,

			.pAttachments = attachments,
			.attachmentCount = 2,

			.width  = extent.width,
			.height = extent.height,

			.layers = 1,
		};

		VkResult vk_result = vkCreateFramebuffer(device.logical_device, &framebuffer_info, null, &framebuffers[i]);
		Assert(vk_result == VK_SUCCESS);
	}
}

void Swapchain::Reload(Window* window, VkRenderPass renderpass) {
	device.WaitIdle();
	Destroy();
	Init(window);
	InitFrameBuffers(renderpass);
}

Optional<u32> Swapchain::GetNextImageIndex(VkSemaphore image_available) {
	u32 index;
	VkResult result = vkAcquireNextImageKHR(device.logical_device, handle, -1, image_available, VK_NULL_HANDLE, &index);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
		return OptNone;
	return index;
}

void Swapchain::Destroy() {
	vkDestroyImageView(device.logical_device, depth_view, null);
	vkDestroyImage(device.logical_device, depth_image, null);
	vkFreeMemory(device.logical_device, depth_memory, null);

	vkDestroySwapchainKHR(device.logical_device, handle, null); // Destroys images.
	images.Reset();

	for (VkFramebuffer framebuffer : framebuffers)
		vkDestroyFramebuffer(device.logical_device, framebuffer, null);
	framebuffers.Reset();

	for (VkImageView view : views)
		vkDestroyImageView(device.logical_device, view, null);
	views.Reset();
}

static SwapchainSupportInfo QuerySwapchainSupportInfo(VkPhysicalDevice pdev, VkSurfaceKHR surface) {
	SwapchainSupportInfo info = {
		.surface = surface,
	};

	// Get capabilities.
	VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pdev, surface, &info.capabilities);
	Assert(result == VK_SUCCESS);

	// Get formats.
	u32 format_count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(pdev, surface, &format_count, null);
	info.formats.AssureCount(format_count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(pdev, surface, &format_count, info.formats.elements);

	// Get present modes.
	u32 present_mode_count;
	vkGetPhysicalDeviceSurfacePresentModesKHR(pdev, surface, &present_mode_count, null);
	info.present_modes.AssureCount(present_mode_count);
	vkGetPhysicalDeviceSurfacePresentModesKHR(pdev, surface, &present_mode_count, info.present_modes.elements);

	return info;
}

