#pragma once
#include "devices.hpp"
#include <vector>
#include <SDL3/SDL_video.h>
#include <vulkan/vulkan.hpp>

struct VulkanSwapChain {
	const VulkanLogicDevice* device;
	SwapChainSupportDetails swapChainDetails;

	VkSwapchainKHR ptr;
	std::vector<VkImage> images;
	std::vector<VkImageView> imageViews;
	std::vector<VkFramebuffer> framebuffers;
	VkFormat format;
	VkExtent2D extents;

	void createFramebuffers(VkRenderPass renderPass);
	void createImageViews();
	void destroy();
	VulkanSwapChain(VkSurfaceKHR surface, SDL_Window* window, const SwapChainSupportDetails& swapChainDetails, const VulkanLogicDevice* device, VkSwapchainKHR old = VK_NULL_HANDLE);
	VulkanSwapChain() {}
};

/// @brief A window that we render to.
struct VulkanSurface {
	VkSurfaceKHR ptr;
	VulkanSwapChain swapChain;
	SDL_Window* targetWindow;
	VkInstance instance;
	VulkanSurface(SDL_Window* window, VkInstance instance);
	VulkanSurface() {}
	void createSwapChain(const SwapChainSupportDetails& swapChainDetails, const VulkanLogicDevice* device, VkSwapchainKHR old = VK_NULL_HANDLE);
	void destroy();
};