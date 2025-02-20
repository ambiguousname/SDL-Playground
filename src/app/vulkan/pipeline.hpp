#pragma once
#include <vulkan/vulkan.hpp>
#include <unordered_map>
#include "surface.hpp"
#include "renderer.hpp"
#include "shader.hpp"

class VulkanRenderer;

// TODO: Some guides online recommend caching pipelines between runs of the application.
struct VulkanPipelineInfo {
	protected:
	VkPipelineInputAssemblyStateCreateInfo inputAssembly;
	VkViewport viewport;
	VkRect2D scissor;
	std::vector<VkDynamicState> dynamicStates;
	VkPipelineDynamicStateCreateInfo dynamicState;
	VkPipelineViewportStateCreateInfo viewportState;
	VkPipelineRasterizationStateCreateInfo rasterizer;
	VkPipelineMultisampleStateCreateInfo multisampling;
	VkPipelineColorBlendAttachmentState colorBlendAttachment;
	VkPipelineColorBlendStateCreateInfo colorBlending;
	VkPipelineLayoutCreateInfo pipelineLayoutInfo;
	ShaderCreationInfo shaderCreationInfo;
	
	public:

	VkGraphicsPipelineCreateInfo pipelineInfo;
	const VulkanLogicDevice* device;
	VkPipelineLayout pipelineLayout;
	
	VulkanPipelineInfo(VulkanRenderer* renderer, ShaderCreationInfo creationInfo);
	ShaderDescription getShaderDescription() { return shaderCreationInfo.getDescription(); }
	void destroy() {
		shaderCreationInfo.destroy();
	}
};

class VulkanPipeline {
	protected:
	// Information for constructing the pipeline:
	VkPipeline ptr = VK_NULL_HANDLE;

	VulkanSurface* surface;
	const VulkanLogicDevice* device;
	public:
	
	VkPipelineLayout pipelineLayout;
	
	VulkanPipeline(VkPipeline ptr, VulkanPipelineInfo creationInfo, VulkanSurface* surface, const VulkanLogicDevice* device, const VulkanPhysicalDevice* physicalDevice);

	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t image_index);
	void destroy();
};