#pragma once
#include <vulkan/vulkan.hpp>
#include "shader.h"
#include "pipeline.h"

struct VulkanPipeline;

struct VulkanObject {
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	VulkanPipeline* parentPipeline;

	const VulkanLogicDevice* device;
	VulkanObject(VulkanPipeline* pipeline, const VulkanLogicDevice* device, const VulkanPhysicalDevice* physicalDevice, VkCommandPool commandPool);
	void destroy();
	void draw(VkCommandBuffer buffer);
};