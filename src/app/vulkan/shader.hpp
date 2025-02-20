#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>
#include "../errors.hpp"

/// Overall information for how a shader will be constructed.
/// Outputs [`ShaderDescription`] as a result.
class ShaderDescription {
	protected:
	public:
	ShaderDescription();
};

struct VulkanShader {
	VkShaderModule shaderModule;
	VkPipelineShaderStageCreateInfo info;
	std::vector<char> bytes;
	VkDevice device;
	VulkanShader(VkDevice device, std::string file_to_load, VkShaderStageFlagBits stage);

	void destroy();
};

struct ShaderCreationInfo {
	std::vector<VkVertexInputBindingDescription> bindings;
	std::vector<VkVertexInputAttributeDescription> attributes;
	
	std::vector<VulkanShader> shaders;
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

	VkPipelineVertexInputStateCreateInfo vertexInfo;

	ShaderCreationInfo(std::vector<VkVertexInputBindingDescription> bindings, std::vector<VkVertexInputAttributeDescription> attributes, std::vector<VulkanShader> shaders);
	ShaderDescription getDescription();
	void destroy();
};