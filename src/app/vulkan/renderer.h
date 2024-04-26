#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>
#include <unordered_set>
#include <SDL_video.h>
#include "devices.h"
#include "surface.h"
#include "shader.h"
#include "helper.h"
#include "pipeline.h"

struct VulkanRenderPass {
	VkRenderPass ptr;
	VulkanRenderPass(VkDevice device, const VulkanSwapChain& swapChain);
	VulkanRenderPass() {}
	void destroy(VkDevice device);
};

class VulkanRenderer {
	VulkanSurface* surface;

	const VulkanLogicDevice* device;
	const VulkanPhysicalDevice* physicalDevice;
	
	VulkanRenderPass renderPass;
	std::vector<VulkanPipeline*> graphicsPipelines;

	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;

	void createCommandPool();
	void recordCommandBuffers(uint32_t image_index);

	VkSemaphore imageAvailable;
	VkSemaphore renderFinished;
	VkFence inFlight;

	void createSync();

	public:
	const VulkanLogicDevice* getDevice() const { return device; }
	const VulkanPhysicalDevice* getPhysicalDevice() const { return physicalDevice; }
	const VkCommandPool getCommandPool() const { return commandPool; }
	const VulkanRenderPass& getRenderPass() const { return renderPass; }
	VulkanSurface* getSurface() const { return surface; } 

	VulkanRenderer() {}
	
	VulkanRenderer(VulkanSurface* surface, const VulkanLogicDevice* device, const VulkanPhysicalDevice* physicalDevice);

	// Take ownership of a VulkanPipeline:
	void attachPipeline(VulkanPipeline* p);
	void refreshSwapChain();
	void draw();
	void destroy();
};