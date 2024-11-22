#pragma once
#include <vulkan/vulkan.hpp>
#include <unordered_map>
#include "surface.hpp"
#include "object.hpp"
#include "renderer.hpp"

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

	std::size_t descriptionHash;
	
	VulkanPipelineInfo(VulkanRenderer* renderer, ShaderDescription description);
	void destroy();
};

// FIXME: This is way too specific to graphics pipelines.
struct VulkanPipeline {
	public:
	VkPipeline ptr = VK_NULL_HANDLE;
	VkPipelineLayout pipelineLayout;
	size_t description_hash;

	VulkanSurface* surface;
	const VulkanLogicDevice* device;

	// FIXME: This would work better as a set, but hashing escapes me for right now.
	std::unordered_set<VulkanObject*> objects;
	
	VulkanPipeline(VkPipeline ptr, VkPipelineLayout layout, VulkanSurface* surface, const VulkanLogicDevice* device) : ptr(ptr), pipelineLayout(layout), surface(surface), device(device) {}

	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t image_index);
	void destroy();

	void attachObject(VulkanObject* o);
	void removeObject(VulkanObject* o);
};