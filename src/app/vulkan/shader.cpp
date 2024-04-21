#include "shader.h"
#include <fstream>
#include "../app.h"

VulkanShader::VulkanShader(VkDevice device, std::string file_to_load, VkShaderStageFlagBits stage) {
	
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