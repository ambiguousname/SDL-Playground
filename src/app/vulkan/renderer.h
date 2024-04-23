#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>
#include <SDL_video.h>
#include "devices.h"
#include "surface.h"
#include "shader.h"

struct VulkanRenderPass {
	VkRenderPass ptr;
	VulkanRenderPass(VkDevice device, const VulkanSwapChain& swapChain);
	VulkanRenderPass() {}
	void destroy(VkDevice device);
};

class VulkanRenderer {
	VulkanSurface* surface;
	VulkanRenderPass renderPass;

	const VulkanLogicDevice* device;
	
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;

	void createCommandPool();

	VkSemaphore imageAvailable;
	VkSemaphore renderFinished;
	VkFence inFlight;

	void createSync();
	void refreshSwapChain();

	void create(VulkanSurface* surface, const VulkanLogicDevice* device, std::vector<VkPipelineShaderStageCreateInfo> shaderStages, VkPipelineVertexInputStateCreateInfo shaderVertexInfo);

	public:
	VulkanRenderer() {}
	template<typename T>
	VulkanRenderer(VulkanSurface* surface, const VulkanLogicDevice* device, ShaderDescription<T> shaderDescription) {
		create(surface, device, shaderDescription.getShaderStages(), shaderDescription.vertexInfo);
		shaderDescription.destroy();
	}

	void draw();
	void recordCommandBuffer(uint32_t image_index);
	void destroy();
};