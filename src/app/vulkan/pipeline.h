#pragma once
#include <vulkan/vulkan.hpp>
#include <unordered_map>
#include "surface.h"
#include "object.h"
#include "renderer.h"

class VulkanRenderer;

// FIXME: This is way too specific to graphics pipelines.
struct VulkanPipeline {
	protected:
	void createPipeline(VulkanRenderer* renderer, std::vector<VkPipelineShaderStageCreateInfo> shaderStages, VkPipelineVertexInputStateCreateInfo shaderVertexInfo);

	public:
	VkPipelineLayout pipelineLayout;
	VkPipeline ptr = VK_NULL_HANDLE;
	size_t description_hash;

	VulkanSurface* surface;
	const VulkanLogicDevice* device;

	std::unordered_set<VulkanObject*> objects;

	VkGraphicsPipelineCreateInfo createInfo;
	
	template<typename T>
	VulkanPipeline(VulkanRenderer* renderer, ShaderDescription<T> description) {
		description_hash = typeid(T).hash_code();

		createPipeline(renderer, description.getShaderStages(), description.vertexInfo);
		
		description.destroy();
	}
	void attachPipelineInstance(VkPipeline ptr) { this->ptr = ptr; }

	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t image_index);
	void destroy();

	void attachObject(VulkanObject* o);
	void removeObject(VulkanObject* o);
};