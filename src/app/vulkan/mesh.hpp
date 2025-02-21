#pragma once
#include "devices.hpp"
#include "pipeline.hpp"
#include "renderer.hpp"
#include "shader.hpp"

#include <glm/glm.hpp>


class VulkanPipeline;
struct VulkanPipelineInfo;
class VulkanRenderer;

struct VulkanVertex {
	glm::vec2 pos;
	glm::vec3 color;
};

typedef glm::mat4 ModelMatrix;

class VulkanMesh : public VulkanObject {
	protected:
	// TODO: Allow multiple pipelines per object.
	/// Pipelines for actually rendering out individual passes on the object:
	VulkanPipeline* pipeline;
	/// Information on how to use the pipeline appropriately during runtime.
	ShaderDescription description;

	VkDescriptorSet transformDescriptorSet;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;

	ModelMatrix model;

	public:
	ModelMatrix* getModel() { return &model; }
	VulkanMesh(VulkanRenderer* renderer, VulkanPipelineInfo creationInfo);
	void destroy() override;
	void draw(VkCommandBuffer buffer, uint32_t image_index) override;
};