#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>
#include <fstream>
#include <glm/glm.hpp>
#include "../errors.h"

template<class T>
concept VertexDescription = requires {
	{T::getBindingDescriptions()};
	{T::getAttributeDescriptions()};
};

struct VulkanVertex {
	glm::vec2 pos;
	glm::vec3 color;	

	static std::vector<VkVertexInputBindingDescription> getBindingDescriptions() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(VulkanVertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		
		return std::vector<VkVertexInputBindingDescription>({bindingDescription});
	}

	static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
		
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(VulkanVertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(VulkanVertex, color);

		return attributeDescriptions;
	}
};

template<VertexDescription T>
struct VulkanShader {
	VkShaderModule shaderModule;
	VkPipelineShaderStageCreateInfo info;
	std::vector<char> bytes;
	VkDevice device;
	VulkanShader(VkDevice device, std::string file_to_load, VkShaderStageFlagBits stage) {
		this->device = device;
		std::ifstream file(file_to_load, std::ios::ate | std::ios::binary);
		if (!file.is_open()) {
			std::string error("Could not open " + file_to_load);
			throw AppError(error);
		}

		size_t fileSize = (size_t) file.tellg();
		bytes.resize(fileSize);
		file.seekg(0);
		file.read(bytes.data(), fileSize);
		file.close();

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = bytes.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(bytes.data());

		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw AppError("Vulkan could not create a shader module.");
		}

		info = VkPipelineShaderStageCreateInfo{};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		info.stage = stage;
		info.module = shaderModule;
		info.pName = "main";
	}

	void destroy() {
		vkDestroyShaderModule(device, shaderModule, nullptr);
	}
};

template<VertexDescription T>
struct ShaderDescription {
	std::vector<VkVertexInputBindingDescription> bindings;
	std::vector<VkVertexInputAttributeDescription> attributes;
	ShaderDescription(std::vector<VulkanShader<T>> shaders) : shaders(shaders) {
		vertexInfo = VkPipelineVertexInputStateCreateInfo{};
		vertexInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		bindings = T::getBindingDescriptions();
		vertexInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindings.size());
		vertexInfo.pVertexBindingDescriptions = bindings.data(); 

		attributes = T::getAttributeDescriptions();	
		vertexInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
		vertexInfo.pVertexAttributeDescriptions = attributes.data();
	}
	std::vector<VulkanShader<T>> shaders;

	std::vector<VkPipelineShaderStageCreateInfo> getShaderStages() {
		std::vector<VkPipelineShaderStageCreateInfo> out;
		for (auto shader : shaders) {
			out.push_back(shader.info);
		}
		return out;
	}

	VkPipelineVertexInputStateCreateInfo vertexInfo;
	void destroy() {
		for (auto shader : shaders) {
			shader.destroy();
		}
	}
};