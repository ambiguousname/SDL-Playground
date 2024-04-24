#pragma once
#include "devices.h"

namespace VulkanHelper {
	void createBuffer(VkDevice device, const VulkanPhysicalDevice* physicalDevice, VkDeviceSize bufferSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer(const VulkanLogicDevice* device, VkCommandPool commandPool, VkBuffer src, VkBuffer dst, VkDeviceSize size);
};