#pragma once
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "object.hpp"

struct DisplayMatrices {
	glm::mat4 projection;
	glm::mat4 view;
};

class VulkanRenderer;

class VulkanCamera : public VulkanObject {
	protected:
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

	void draw(VkCommandBuffer buffer, uint32_t image_index) override;
	void destroy() override;

	glm::mat4* getView() { return &display.view; }
	glm::mat4* getProj() { return &display.projection; }

	void translate(glm::vec3 add);
};