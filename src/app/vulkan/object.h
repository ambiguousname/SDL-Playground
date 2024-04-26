#pragma once
#include <vulkan/vulkan.hpp>
#include "shader.h"
#include "renderer.h"
#include "pipeline.h"

class VulkanRenderer;
struct VulkanPipeline;

struct VulkanObject {
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	const VulkanLogicDevice* device;
	VulkanObject(VulkanPipeline* pipeline, const VulkanLogicDevice* device, const VulkanPhysicalDevice* physicalDevice, VkCommandPool commandPool);
	void destroy();
	void draw(VkCommandBuffer buffer);
};