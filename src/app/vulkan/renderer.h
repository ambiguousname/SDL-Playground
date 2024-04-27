#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>
#include <unordered_set>
#include <unordered_map>
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
	std::unordered_map<size_t, VulkanPipeline> graphicsPipelines;

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
	VulkanPipeline* getPipeline(size_t hash) { if (auto find = graphicsPipelines.find(hash); find != graphicsPipelines.end()) { return &find->second; } else { return nullptr; } }

	VulkanRenderer() {}
	
	VulkanRenderer(VulkanSurface* surface, const VulkanLogicDevice* device, const VulkanPhysicalDevice* physicalDevice);

	// Take ownership of a VulkanPipeline:
	void attachPendingGraphicsPipeline(VulkanPipeline pipeline);
	void intializePipelines();

	void refreshSwapChain();
	void draw();
	void destroy();
};