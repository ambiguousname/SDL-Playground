#pragma once
#include <vulkan/vulkan.hpp>
#include "surface.h"
#include "object.h"

struct VulkanRenderPass {
	VkRenderPass ptr;
	VulkanRenderPass(VkDevice device, const VulkanSwapChain& swapChain);
	VulkanRenderPass() {}
	void destroy(VkDevice device);
};

struct VulkanPipeline {
	VkPipelineLayout pipelineLayout;
	VkPipeline ptr;

	VulkanSurface* surface;
	const VulkanLogicDevice* device;

	std::unordered_set<VulkanObject*> objects;
	
	VulkanRenderPass renderPass;
	VulkanPipeline(VulkanSurface* surface, const VulkanLogicDevice* device, std::vector<VkPipelineShaderStageCreateInfo> shaderStages, VkPipelineVertexInputStateCreateInfo shaderVertexInfo);
	void refreshSwapChain();
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t image_index);
	void destroy();

	void attachObject(VulkanObject* o);
	void removeObject(VulkanObject* o);
};