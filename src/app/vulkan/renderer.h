#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>
#include <SDL_video.h>
#include "devices.h"

struct VulkanSwapChain {
	const VulkanLogicDevice* device;

	VkSwapchainKHR ptr;
	std::vector<VkImage> images;
	std::vector<VkImageView> imageViews;
	std::vector<VkFramebuffer> framebuffers;
	VkFormat format;
	VkExtent2D extents;

	void createFramebuffers(VkRenderPass renderPass);
	void createImageViews();
	void destroy();
	VulkanSwapChain(SDL_Window* window, VkSurfaceKHR surface, const SwapChainSupportDetails& swapChainDetails, const VulkanLogicDevice* device);
	VulkanSwapChain() {}
};

struct VulkanRenderPass {
	VkRenderPass ptr;
	VulkanRenderPass(VkDevice device, const VulkanSwapChain& swapChain);
	VulkanRenderPass() {}
	void destroy(VkDevice device);
};

class VulkanRenderer {
	VulkanSwapChain swapChain;
	VulkanRenderPass renderPass;

	const VulkanLogicDevice* device;
	
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;

	void createCommandPool();

	VkSemaphore imageAvailable;
	VkSemaphore renderFinished;
	VkFence inFlight;

	void createSync();

	public:
	VulkanRenderer() {}
	VulkanRenderer(VulkanSwapChain swapChain, const VulkanLogicDevice* device);

	void draw();
	void recordCommandBuffer(uint32_t image_index);
	void destroy();
};