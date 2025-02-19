#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>
#include "../errors.hpp"

struct VulkanShader {
	VkShaderModule shaderModule;
	VkPipelineShaderStageCreateInfo info;
	std::vector<char> bytes;
	VkDevice device;
	VulkanShader(VkDevice device, std::string file_to_load, VkShaderStageFlagBits stage);

	void destroy();
};

struct ShaderDescription {
	std::vector<VkVertexInputBindingDescription> bindings;
	std::vector<VkVertexInputAttributeDescription> attributes;
	
	std::vector<VulkanShader> shaders;

	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

	VkPipelineVertexInputStateCreateInfo vertexInfo;
	
	ShaderDescription(std::vector<VkVertexInputBindingDescription> bindings, std::vector<VkVertexInputAttributeDescription> attributes, std::vector<VulkanShader> shaders);
	void destroy();
};