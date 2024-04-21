#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>

struct VulkanShader {
	VkShaderModule shaderModule;
	VkPipelineShaderStageCreateInfo info;
	std::vector<char> bytes;
	VulkanShader(VkDevice device, std::string file_to_load, VkShaderStageFlagBits stage);
};