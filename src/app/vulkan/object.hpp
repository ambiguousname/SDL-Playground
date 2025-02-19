#pragma once
#include <vulkan/vulkan.hpp>
#include "devices.hpp"
#include "pipeline.hpp"

#include <glm/glm.hpp>


class VulkanPipeline;
struct VulkanPipelineInfo;

struct VulkanVertex {
	glm::vec2 pos;
	glm::vec3 color;
};

struct DisplayMatrices {
	glm::mat4 view;
	glm::mat4 proj;
};

typedef glm::mat4 ModelMatrix;

class VulkanObject {
	protected:
	// TODO: Allow multiple pipelines per object.
	// Pipelines for actually rendering out individual passes on the object:
	VulkanPipeline* pipeline;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	// Descriptor sets for the object:
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

	VkDescriptorSetLayout descriptorSetLayout;

	const VulkanLogicDevice* device;
	
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;

	glm::mat4 modelMatrix;

	public:
	VulkanObject(const VulkanLogicDevice* device, const VulkanPhysicalDevice* physicalDevice, VulkanSurface* surface, VulkanPipelineInfo creationInfo, VkCommandPool commandPool);
	void destroy();
	void draw(VkCommandBuffer buffer, uint32_t image_index);
};