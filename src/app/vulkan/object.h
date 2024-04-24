#pragma once
#include <vulkan/vulkan.hpp>
#include "shader.h"
#include "renderer.h"

class VulkanRenderer;

struct VulkanObject {
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	const VulkanLogicDevice* device;
	VulkanObject(VulkanRenderer* renderer);
	void destroy();
	void draw(VkCommandBuffer buffer);
};