#pragma once
#include <vulkan/vulkan.hpp>
#include "surface.h"
#include "object.h"

struct VulkanPipeline {
	VkPipelineLayout pipelineLayout;
	VkPipeline ptr;

	VulkanSurface* surface;
	const VulkanLogicDevice* device;

	std::unordered_set<VulkanObject*> objects;
	
	static VkGraphicsPipelineCreateInfo getPipelineInfo(VulkanRenderer* renderer, std::vector<VkPipelineShaderStageCreateInfo> shaderStages, VkPipelineVertexInputStateCreateInfo shaderVertexInfo);
	VulkanPipeline(VulkanRenderer* renderer, VkPipeline ptr, VkPipelineLayout layout);
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t image_index);
	void destroy();

	void attachObject(VulkanObject* o);
	void removeObject(VulkanObject* o);
};