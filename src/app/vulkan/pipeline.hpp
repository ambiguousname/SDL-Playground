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
	VkDescriptorSetLayout descriptorSetLayout;

	std::size_t descriptionHash;
	
	VulkanPipelineInfo(VulkanRenderer* renderer, ShaderDescription description);
	void destroy();
};

// TODO: Need to make this more flexible
struct DisplayMatrices {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
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

	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

	VkDescriptorSetLayout descriptorSetLayout;

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;

	glm::mat4 modelMatrix;
	
	VulkanPipeline(VkPipeline ptr, VulkanPipelineInfo* creationInfo, VulkanSurface* surface, const VulkanLogicDevice* device, const VulkanPhysicalDevice* physicalDevice);

	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t image_index);
	void destroy();

	void attachObject(VulkanObject* o);
	void removeObject(VulkanObject* o);
};