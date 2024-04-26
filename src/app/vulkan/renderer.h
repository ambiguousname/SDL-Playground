#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>
#include <unordered_set>
#include <SDL_video.h>
#include "devices.h"
#include "surface.h"
#include "shader.h"
#include "helper.h"
#include "pipeline.h"

class VulkanRenderer {
	VulkanSurface* surface;

	const VulkanLogicDevice* device;
	const VulkanPhysicalDevice* physicalDevice;
	
	std::vector<VulkanPipeline> graphicsPipelines;

	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;

	void createCommandPool();

	VkSemaphore imageAvailable;
	VkSemaphore renderFinished;
	VkFence inFlight;

	void createSync();

	void create(VulkanSurface* surface, const VulkanLogicDevice* device);

	public:
	const VulkanLogicDevice* getDevice() const { return device; }
	const VulkanPhysicalDevice* getPhysicalDevice() const { return physicalDevice; }
	const VkCommandPool getCommandPool() const { return commandPool; }

	VulkanRenderer() {}
	
	template<typename T>
	VulkanRenderer(const VulkanSurface* surface, const VulkanLogicDevice* device, const VulkanPhysicalDevice* physicalDevice, ShaderDescription<T> shaderDescription) {
		this->physicalDevice = physicalDevice;
		create(surface, device, shaderDescription.getShaderStages(), shaderDescription.vertexInfo);
		shaderDescription.destroy();
	}

	void draw();
	void destroy();
};