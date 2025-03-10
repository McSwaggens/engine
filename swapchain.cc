#include "swapchain.h"

#include "vk_helper.h"

void Swapchain::InitImages(VkDevice device) {
	images.Reset();

	u32 image_count;
	vkGetSwapchainImagesKHR(device, handle, &image_count, null);

	images.AssureCount(image_count);
	vkGetSwapchainImagesKHR(device, handle, &image_count, images.elements);
}

void Swapchain::InitViews(VkDevice device) {
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

		vkCreateImageView(device, &image_view_create_info, null, &views[i]);
	}
}

void Swapchain::InitFrameBuffers(VkDevice device, VkRenderPass renderpass) {
	framebuffers.AssureCount(images.count);

	for (u32 i = 0; i < images.count; i++) {
		VkImageView attachments[] = {
			views[i],
		};

		VkFramebufferCreateInfo framebuffer_info = {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.renderPass = renderpass,

			.pAttachments = attachments,
			.attachmentCount = 1,

			.width  = extent.width,
			.height = extent.height,

			.layers = 1,
		};

		VkResult vk_result = vkCreateFramebuffer(device, &framebuffer_info, null, &framebuffers[i]);
		Assert(vk_result == VK_SUCCESS);
	}
}

void Swapchain::Destroy(VkDevice device) {
	vkDestroySwapchainKHR(device, handle, null); // Destroys images.
	images.Reset();

	for (VkFramebuffer framebuffer : framebuffers)
		vkDestroyFramebuffer(device, framebuffer, null);

	for (VkImageView view : views)
		vkDestroyImageView(device, view, null);
	views.Reset();
}
