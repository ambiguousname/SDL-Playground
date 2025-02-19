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
	ShaderDescription description;
	
	public:

	VkGraphicsPipelineCreateInfo pipelineInfo;
	const VulkanLogicDevice* device;
	VkPipelineLayout pipelineLayout;
	VkDescriptorSetLayout descriptorSetLayout;
	
	VulkanPipelineInfo(VulkanRenderer* renderer, ShaderDescription description);
	void destroy();
};

// TODO: Need to make this more flexible
struct DisplayMatrices {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
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