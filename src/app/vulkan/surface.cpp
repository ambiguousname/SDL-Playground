#include "surface.h"
#include "../app.h"
#include <SDL_vulkan.h>


void VulkanSwapChain::destroy() {
	for (auto imageView : imageViews) {
		vkDestroyImageView(device->ptr, imageView, nullptr);
	}
	for (auto framebuffer : framebuffers) {
		vkDestroyFramebuffer(device->ptr, framebuffer, nullptr);
	}
	vkDestroySwapchainKHR(device->ptr, ptr, nullptr);
}

void VulkanSwapChain::createImageViews() {
	imageViews.resize(images.size());
	for (size_t i = 0; i < images.size(); i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = images[i];

		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = format;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		if (vkCreateImageView(device->ptr, &createInfo, nullptr, &imageViews[i]) != VK_SUCCESS) {
			throw AppError("Vulkan failed to create image views.");
		}
	}
}

void VulkanSwapChain::createFramebuffers(VkRenderPass renderPass) {
	framebuffers.resize(imageViews.size());

	for (size_t i = 0; i < imageViews.size(); i++) {
		VkImageView attachments[] = {
			imageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = extents.width;
		framebufferInfo.height = extents.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device->ptr, &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
			throw AppError("Vulkan could not create a framebuffer.");
		}
	}
}

VulkanSwapChain::VulkanSwapChain(VkSurfaceKHR surface, SDL_Window* window, const SwapChainSupportDetails& swapChainDetails, const VulkanLogicDevice* device) {
	this->device = device;
	this->swapChainDetails = SwapChainSupportDetails(swapChainDetails);
	VkSurfaceFormatKHR format = swapChainDetails.formats[0];
	for (const auto& aFormat : swapChainDetails.formats) {
		if (aFormat.format == VK_FORMAT_B8G8R8_SRGB && aFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			format = aFormat;
		}
	}
	
	this->format = format.format;

	VkPresentModeKHR mode = VK_PRESENT_MODE_FIFO_KHR;
	for (const auto& aMode : swapChainDetails.presentModes) {
		if (aMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			mode = aMode;
		}
	}

	this->extents = swapChainDetails.capabilities.currentExtent;
	if (extents.width == std::numeric_limits<uint32_t>::max()) {
		int width, height;
		SDL_GetWindowSizeInPixels(window, &width, &height);

		extents.width = std::clamp(static_cast<uint32_t>(width), swapChainDetails.capabilities.minImageExtent.width, swapChainDetails.capabilities.maxImageExtent.width);
		extents.height = std::clamp(static_cast<uint32_t>(height), swapChainDetails.capabilities.minImageExtent.height, swapChainDetails.capabilities.maxImageExtent.height);
	}

	uint32_t imageCount = swapChainDetails.capabilities.minImageCount + 1;
	if (swapChainDetails.capabilities.maxImageCount > 0 && imageCount > swapChainDetails.capabilities.maxImageCount) {
		imageCount = swapChainDetails.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = format.format;
	createInfo.imageColorSpace = format.colorSpace;
	createInfo.imageExtent = extents;
	// Always set to one, unless 3D stereoscopic image.
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t queueFamilyIndices[] = {device->graphicsFamily,device->presentFamily};

	if (device->graphicsFamily != device->presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	} else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	// In case we want to transform the image in any way before rendering:
	createInfo.preTransform = swapChainDetails.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = mode;
	createInfo.clipped = VK_TRUE;

	// TODO: This needs to allow for window resizing.
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(device->ptr, &createInfo, nullptr, &ptr) != VK_SUCCESS) {
		throw AppError("Vulkan could not create a swap chain.");
	}


	vkGetSwapchainImagesKHR(device->ptr, ptr, &imageCount, nullptr);
	images.resize(imageCount);
	vkGetSwapchainImagesKHR(device->ptr, ptr, &imageCount, images.data());

	createImageViews();
}

VulkanSurface::VulkanSurface(SDL_Window* window, VkInstance instance) {
	targetWindow = window;
	this->instance = instance;

	bool success = SDL_Vulkan_CreateSurface(window, instance, &ptr);
	if (!success) {
		throw SDLError("SDL could not create a Vulkan Surface.");
	}
}

void VulkanSurface::createSwapChain(const SwapChainSupportDetails& swapChainDetails, const VulkanLogicDevice* device) {
	swapChain = VulkanSwapChain(ptr, targetWindow, swapChainDetails, device);
}

void VulkanSurface::destroy() {
	swapChain.destroy();
	vkDestroySurfaceKHR(instance, ptr, nullptr);
}