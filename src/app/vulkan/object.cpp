#include "object.hpp"
#include "helper.hpp"
#include "pipeline.hpp"

const std::vector<VulkanVertex> vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
};

// TODO: Multiple created objects need to be made into one allocation.
// Indices should also be merged into the same buffer as an index.
VulkanObject::VulkanObject(const VulkanLogicDevice* device, const VulkanPhysicalDevice* physicalDevice, VulkanSurface* surface, VulkanPipelineInfo creationInfo, VkCommandPool commandPool) : device(device), descriptorSetLayout(creationInfo.descriptorSetLayout) {
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	VkDeviceSize size = sizeof(vertices[0]) * vertices.size();
	VulkanHelper::createBuffer(device->ptr, physicalDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device->ptr, stagingBufferMemory, 0, size, 0, &data);
	memcpy(data, vertices.data(), (size_t) size);
	vkUnmapMemory(device->ptr, stagingBufferMemory);

	VulkanHelper::createBuffer(device->ptr, physicalDevice, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
	
	VulkanHelper::copyBuffer(device, commandPool, stagingBuffer, vertexBuffer, size);

	vkDestroyBuffer(device->ptr, stagingBuffer, nullptr);
	vkFreeMemory(device->ptr, stagingBufferMemory, nullptr);

	VkDeviceSize indexSize = sizeof(indices[0]) * indices.size();
	VkBuffer indexStaging;
	VkDeviceMemory indexStagingMemory;

	VulkanHelper::createBuffer(device->ptr, physicalDevice, indexSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), indexStaging, indexStagingMemory);

	void* indexDat;
	vkMapMemory(device->ptr, indexStagingMemory, 0, indexSize, 0, &indexDat);
	memcpy(indexDat, indices.data(), (size_t) size);
	vkUnmapMemory(device->ptr, indexStagingMemory);

	VulkanHelper::createBuffer(device->ptr, physicalDevice, indexSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);
	VulkanHelper::copyBuffer(device, commandPool, indexStaging, indexBuffer, indexSize);

	vkDestroyBuffer(device->ptr, indexStaging, nullptr);
	vkFreeMemory(device->ptr, indexStagingMemory, nullptr);

	
	// TODO: Allow multiple pipelines, caching.
	std::vector<VkGraphicsPipelineCreateInfo> creation({creationInfo.pipelineInfo});
	std::vector<VkPipeline> pipelinePtrs(creation.size());

	if (vkCreateGraphicsPipelines(device->ptr, VK_NULL_HANDLE, creation.size(), creation.data(), nullptr, pipelinePtrs.data()) != VK_SUCCESS) {
		throw AppError("Vulkan could not create graphics pipelines.");
	}

	pipeline = new VulkanPipeline(pipelinePtrs[0], creationInfo, surface, device, physicalDevice);
	
	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	// TODO: https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
	poolSize.descriptorCount = 1;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	// TODO: https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
	poolInfo.maxSets = 1;

	if (vkCreateDescriptorPool(device->ptr, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw AppError("Vulkan could not create descriptor pool.");
	}

	// TODO: https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
	std::vector<VkDescriptorSetLayout> layouts(1, descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	// TODO: https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = layouts.data();

	// TODO: https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
	descriptorSets.resize(1);
	if (vkAllocateDescriptorSets(device->ptr, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
		throw AppError("Vulkan could not create descriptor sets.");
	}

	
	// TODO: https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
	// Uniform buffers:
	VkDeviceSize bufferSize = sizeof(DisplayMatrices);
	uniformBuffers.resize(1);
	uniformBuffersMemory.resize(1);
	uniformBuffersMapped.resize(1);

	for (size_t i = 0; i < 1; i++) {
		VulkanHelper::createBuffer(device->ptr, physicalDevice, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);

		vkMapMemory(device->ptr, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);

		VkDescriptorBufferInfo bufferInfo{};
		// TODO: https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
		bufferInfo.buffer = uniformBuffers[0];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(DisplayMatrices);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		// TODO: https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
		descriptorWrite.dstSet = descriptorSets[0];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = nullptr; // Optional
		descriptorWrite.pTexelBufferView = nullptr; // Optional
		vkUpdateDescriptorSets(device->ptr, 1, &descriptorWrite, 0, nullptr);
	}

	creationInfo.destroy();
}

void VulkanObject::destroy() {
	pipeline->destroy();
	delete pipeline;

	// TODO: https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
	for (size_t i = 0; i < 1; i++) {
        vkDestroyBuffer(device->ptr, uniformBuffers[i], nullptr);
        vkFreeMemory(device->ptr, uniformBuffersMemory[i], nullptr);
    }

	vkDestroyDescriptorPool(device->ptr, descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(device->ptr, descriptorSetLayout, nullptr);
	
	vkDestroyBuffer(device->ptr, vertexBuffer, nullptr);
	vkFreeMemory(device->ptr, vertexBufferMemory, nullptr);

	vkDestroyBuffer(device->ptr, indexBuffer, nullptr);
	vkFreeMemory(device->ptr, indexBufferMemory, nullptr);
}

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
void VulkanObject::draw(VkCommandBuffer buffer, uint32_t image_index) {
	VkBuffer vertexBuffers[] = {vertexBuffer};
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(buffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(buffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
	
	// TODO: Make this flexible.
	DisplayMatrices test{};
	test.model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	test.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	test.proj = glm::perspective(glm::radians(45.0f), 0.5f, 0.1f, 10.0f);
	// test.proj[1][1] *= -1;
	
	// TODO: https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
	memcpy(uniformBuffersMapped[0], &test, sizeof(test));

	// TODO: https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
	vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipelineLayout, 0, 1, &descriptorSets[0], 0, nullptr);
	
	pipeline->recordCommandBuffer(buffer, image_index);
	vkCmdDrawIndexed(buffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}