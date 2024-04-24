#pragma once
#include <vulkan/vulkan.hpp>
#include "shader.h"

struct VulkanObject {
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkDevice device;
	VulkanObject(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties);
	VulkanObject() {}
	void destroy();
	void draw(VkCommandBuffer buffer);
};