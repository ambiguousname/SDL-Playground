#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>
#include <SDL_video.h>
#include "devices.h"
#include "surface.h"

struct VulkanRenderPass {
	VkRenderPass ptr;
	VulkanRenderPass(VkDevice device, const VulkanSwapChain& swapChain);
	VulkanRenderPass() {}
	void destroy(VkDevice device);
};

class VulkanRenderer {
	VulkanSurface* surface;
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
	void refreshSwapChain();

	public:
	VulkanRenderer() {}
	VulkanRenderer(VulkanSurface* surface, const VulkanLogicDevice* device);

	void draw();
	void recordCommandBuffer(uint32_t image_index);
	void destroy();
};