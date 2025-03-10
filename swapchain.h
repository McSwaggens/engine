#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include "general.h"
#include "list.h"

struct Swapchain {
	VkSwapchainKHR handle = 0;

	VkSurfaceFormatKHR surface_format = { };
	VkExtent2D extent = { 0, 0 };

	List<VkImage>     images;
	List<VkImageView> views;

	List<VkFramebuffer> framebuffers;

	void InitImages(VkDevice device);
	void InitViews(VkDevice device);
	void InitFrameBuffers(VkDevice device, VkRenderPass renderpass);
	void Destroy(VkDevice device);
};

#endif // SWAPCHAIN_H
