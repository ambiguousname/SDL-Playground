#pragma once
#include "devices.hpp"

class VulkanRenderer;

/// @brief Common list of 
class VulkanObject {
	protected:
	const VulkanLogicDevice* device;

	public:
	VulkanObject(VulkanRenderer* renderer);
	virtual void destroy() {}
	virtual void draw(VkCommandBuffer buffer, uint32_t image_index) {}
};