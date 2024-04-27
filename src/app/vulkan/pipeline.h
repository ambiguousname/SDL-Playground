#pragma once
#include <vulkan/vulkan.hpp>
#include <unordered_map>
#include "surface.h"
#include "object.h"
#include "renderer.h"

class VulkanRenderer;

// TODO: Some guides online recommend caching pipelines between runs of the application.
struct VulkanPipelineInfo {
	protected:
	void createPipelineInfo(VulkanRenderer* renderer, std::vector<VkPipelineShaderStageCreateInfo> shaderStages, VkPipelineVertexInputStateCreateInfo shaderVertexInfo);
	
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
	VkPipelineLayout pipelineLayout;

	public:
	const VulkanLogicDevice* device;
	VkGraphicsPipelineCreateInfo pipelineInfo;
	size_t descriptionHash;
	template<typename T>
	VulkanPipelineInfo(VulkanRenderer* renderer, ShaderDescription<T> description) {
		descriptionHash = typeid(T).hash_code();

		createPipelineInfo(renderer, description.getShaderStages(), description.vertexInfo);
		
		description.destroy();
	}

	void destroy();
};

// FIXME: This is way too specific to graphics pipelines.
struct VulkanPipeline {
	public:
	VkPipeline ptr = VK_NULL_HANDLE;

	VulkanSurface* surface;
	const VulkanLogicDevice* device;

	std::unordered_set<VulkanObject*> objects;
	
	VulkanPipeline(VkPipeline ptr) : ptr(ptr) {}

	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t image_index);
	void destroy();

	void attachObject(VulkanObject* o);
	void removeObject(VulkanObject* o);
};