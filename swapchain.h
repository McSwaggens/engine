#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include "general.h"
#include "list.h"
#include "optional.h"
#include "window.h"
#include "vector.h"

struct Swapchain {
	VkSwapchainKHR handle = 0;

	VkSurfaceFormatKHR surface_format = { };
	VkExtent2D extent = { 0, 0 };

	List<VkImage>     images;
	List<VkImageView> views;

	List<VkFramebuffer> framebuffers;

	void Init(Window* window);
	void InitImages();
	void InitViews();
	void InitFrameBuffers(VkRenderPass renderpass);

	void Reload(Window* window, VkRenderPass renderpass);

	Optional<u32> GetNextImageIndex(VkSemaphore image_available);

	void Destroy();
};

struct SwapchainSupportInfo {
	VkSurfaceKHR surface;
	VkSurfaceCapabilitiesKHR capabilities;
	List<VkSurfaceFormatKHR> formats;
	List<VkPresentModeKHR>   present_modes;

	VkPresentModeKHR ChoosePresentMode() {
		if (present_modes.Contains(VK_PRESENT_MODE_MAILBOX_KHR))
			return VK_PRESENT_MODE_MAILBOX_KHR;

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkSurfaceFormatKHR ChooseFormat() {
		if (!formats.Contains({ VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR }))
			return { VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

		return formats[0];
	}

	VkExtent2D GetExtent(Window* window) {
		if (capabilities.currentExtent.width != -1)
			return capabilities.currentExtent;

		Vector2 fb_size = window->GetFrameBufferSize();
		return {
			Clamp((u32)fb_size.x, capabilities.minImageExtent.width,  capabilities.maxImageExtent.width),
			Clamp((u32)fb_size.y, capabilities.minImageExtent.height, capabilities.maxImageExtent.height),
		};
	}

	u32 GetImageCount() {
		u32 image_count = capabilities.minImageCount + 1;

		if (capabilities.maxImageCount > 0)
			image_count = Min(image_count, capabilities.maxImageCount);

		return image_count;
	}


	void Free() {
		formats.Free();
		present_modes.Free();
	}
};

static SwapchainSupportInfo QuerySwapchainSupportInfo(VkPhysicalDevice pdev, VkSurfaceKHR surface);

#endif // SWAPCHAIN_H
