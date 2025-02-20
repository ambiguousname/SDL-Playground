#pragma once
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <vulkan/vulkan.hpp>
#include "renderer.hpp"

struct DisplayMatrices {
	glm::mat4 projection;
	glm::mat4 view;
};

class VulkanRenderer;

class VulkanCamera {
	protected:
	const VulkanLogicDevice* device;
	VkBuffer projectionBuffer;
	VkDeviceMemory projectionBufferMemory;
	void* projectionBufferMapped;

	DisplayMatrices display;

	glm::vec3 position = glm::vec3(2.0f, 2.0f, 2.0f);
	glm::vec3 forward = glm::vec3(-2.0f, -2.0f, -2.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	float fov;
	float nearZ;
	float farZ;
	float w;
	float h;

	public:
	VulkanCamera(VulkanRenderer* renderer, float w, float h, float fov=45.0f, float nearZ=0.1f, float farZ=100.0f);

	void draw();
	void destroy();

	void translate(glm::vec3 add);
};