#include "camera.hpp"


VulkanCamera::VulkanCamera(VulkanRenderer* renderer, float w, float h, float fov, float nearZ, float farZ) : device(renderer->getDevice()), w(w), h(h), fov(fov), nearZ(nearZ), farZ(farZ)  {
	updateProjection();
	update();

	const VulkanPhysicalDevice* physicalDevice = renderer->getPhysicalDevice();

	
	VkDeviceSize bufferSize = sizeof(DisplayMatrices);
	for (size_t i = 0; i < 1; i++) {
		VulkanHelper::createBuffer(device->ptr, physicalDevice, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, projectionBuffer, projectionBufferMemory);

		vkMapMemory(device->ptr, projectionBufferMemory, 0, bufferSize, 0, &projectionBufferMapped);

		VkDescriptorBufferInfo bufferInfo{};
		// TODO: https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
		bufferInfo.buffer = projectionBuffer;
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(DisplayMatrices) + sizeof(ModelMatrix);

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

void VulkanCamera::draw() {
	// TODO: Make this flexible.
	DisplayMatrices test{};
	test.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	test.projection = glm::perspective(glm::radians(45.0f), 0.5f, 0.1f, 10.0f);
	// test.proj[1][1] *= -1;
	memcpy(projectionBufferMapped, &test, sizeof(DisplayMatrices));
}

void VulkanCamera::destroy() {
	vkDestroyBuffer(device->ptr, projectionBuffer, nullptr);
	vkFreeMemory(device->ptr, projectionBufferMemory, nullptr);
}