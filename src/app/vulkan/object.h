#pragma once
#include <vulkan/vulkan.hpp>
#include <unordered_set>
#include "shader.h"
#include "devices.h"

struct VulkanObject {
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	const VulkanLogicDevice* device;
	VulkanObject(const VulkanLogicDevice* device, const VulkanPhysicalDevice* physicalDevice, VkCommandPool commandPool);
	void destroy();
	void draw(VkCommandBuffer buffer);
};