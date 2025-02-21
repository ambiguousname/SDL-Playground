#include "camera.hpp"
#include "renderer.hpp"


VulkanCamera::VulkanCamera(VulkanRenderer* renderer, float w, float h, float fov, float nearZ, float farZ) : VulkanObject(renderer), w(w), h(h), fov(fov), nearZ(nearZ), farZ(farZ)  {
	display.projection = glm::perspective(glm::radians(fov), w/h, nearZ, farZ);
	display.view = glm::lookAt(position, position + forward, up);

	const VulkanPhysicalDevice* physicalDevice = renderer->getPhysicalDevice();

	
	VkDeviceSize bufferSize = sizeof(DisplayMatrices);
	for (size_t i = 0; i < 1; i++) {
		VulkanHelper::createBuffer(device->ptr, physicalDevice, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, projectionBuffer, projectionBufferMemory);

		vkMapMemory(device->ptr, projectionBufferMemory, 0, bufferSize, 0, &projectionBufferMapped);

		VkDescriptorBufferInfo bufferInfo{};
		// TODO: https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
		bufferInfo.buffer = projectionBuffer;
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(DisplayMatrices);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		// TODO: https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
		descriptorWrite.dstSet = renderer->getDescriptorSet(VulkanRenderer::DescriptorKind::TRANSFORMS);
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = nullptr; // Optional
		descriptorWrite.pTexelBufferView = nullptr; // Optional
		vkUpdateDescriptorSets(device->ptr, 1, &descriptorWrite, 0, nullptr);
	}
}

void VulkanCamera::translate(glm::vec3 add)  {
	position += add;
	glm::translate(display.view, -add);
}

void VulkanCamera::draw(VkCommandBuffer buffer, uint32_t image_index) {
	memcpy(projectionBufferMapped, &display, sizeof(DisplayMatrices));
}

void VulkanCamera::destroy() {
	vkDestroyBuffer(device->ptr, projectionBuffer, nullptr);
	vkFreeMemory(device->ptr, projectionBufferMemory, nullptr);
}