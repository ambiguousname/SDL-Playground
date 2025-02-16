#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <SDL3/SDL_video.h>
#include "devices.hpp"
#include "surface.hpp"
#include "shader.hpp"
#include "helper.hpp"
#include "pipeline.hpp"

struct VulkanRenderPass {
	VkRenderPass ptr;
	VulkanRenderPass(VkDevice device, const VulkanSwapChain& swapChain);
	VulkanRenderPass() {}
	void destroy(VkDevice device);
};

struct VulkanPipeline;
struct VulkanPipelineInfo;

class VulkanRenderer {
	VulkanSurface* surface;

	const VulkanLogicDevice* device;
	const VulkanPhysicalDevice* physicalDevice;
	
	VulkanRenderPass renderPass;
	std::unordered_map<size_t, VulkanPipeline*> graphicsPipelines;
	std::vector<VulkanPipelineInfo*> creationInfo;

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
	VulkanPipeline* getPipeline(std::size_t hash) { if (auto find = graphicsPipelines.find(hash); find != graphicsPipelines.end()) { return find->second; } else { return nullptr; } }

	VulkanRenderer() {}
	
	VulkanRenderer(VulkanSurface* surface, const VulkanLogicDevice* device, const VulkanPhysicalDevice* physicalDevice);

	// Take ownership of a VulkanPipeline:
	void attachPendingGraphicsPipeline(VulkanPipelineInfo* info);
	void intializePipelines();

	void refreshSwapChain();
	void draw();
	void destroy();
};