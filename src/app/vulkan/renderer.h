#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>
#include <SDL_video.h>
#include "devices.h"
#include "surface.h"
#include "shader.h"
#include "object.h"
#include "helper.h"
#include <unordered_set>

struct VulkanRenderPass {
	VkRenderPass ptr;
	VulkanRenderPass(VkDevice device, const VulkanSwapChain& swapChain);
	VulkanRenderPass() {}
	void destroy(VkDevice device);
};

class VulkanObject;

class VulkanRenderer {
	VulkanSurface* surface;
	VulkanRenderPass renderPass;

	const VulkanLogicDevice* device;
	const VulkanPhysicalDevice* physicalDevice;
	
	VkPipelineLayout pipelineLayout;
	// TODO: There should be one pipeline per set of shaders.
	VkPipeline graphicsPipeline;

	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;

	void createCommandPool();

	VkSemaphore imageAvailable;
	VkSemaphore renderFinished;
	VkFence inFlight;

	std::unordered_set<VulkanObject*> objects;

	void createSync();
	void refreshSwapChain();

	void create(VulkanSurface* surface, const VulkanLogicDevice* device, std::vector<VkPipelineShaderStageCreateInfo> shaderStages, VkPipelineVertexInputStateCreateInfo shaderVertexInfo);

	public:
	const VulkanLogicDevice* getDevice() const { return device; }
	const VulkanPhysicalDevice* getPhysicalDevice() const { return physicalDevice; }
	const VkCommandPool getCommandPool() const { return commandPool; }

	VulkanRenderer() {}
	
	template<typename T>
	VulkanRenderer(VulkanSurface* surface, const VulkanLogicDevice* device, const VulkanPhysicalDevice* physicalDevice, ShaderDescription<T> shaderDescription) {
		this->physicalDevice = physicalDevice;
		create(surface, device, shaderDescription.getShaderStages(), shaderDescription.vertexInfo);
		shaderDescription.destroy();
	}

	void attachObject(VulkanObject* o);
	void removeObject(VulkanObject* o);

	void draw();
	void recordCommandBuffer(uint32_t image_index);
	void destroy();
};