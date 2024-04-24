#include "object.h"
#include "helper.h"

const std::vector<VulkanVertex> vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
};

VulkanObject::VulkanObject(VulkanRenderer* renderer) {
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	this->device = renderer->getDevice();
	const VulkanPhysicalDevice* physicalDevice = renderer->getPhysicalDevice();

	VkDeviceSize size = sizeof(vertices[0]) * vertices.size();
	VulkanHelper::createBuffer(device->ptr, physicalDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device->ptr, stagingBufferMemory, 0, size, 0, &data);
	memcpy(data, vertices.data(), (size_t) size);
	vkUnmapMemory(device->ptr, stagingBufferMemory);

	VulkanHelper::createBuffer(device->ptr, physicalDevice, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
	
	VkCommandPool commandPool = renderer->getCommandPool();
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

	renderer->attachObject(this);
}

void VulkanObject::destroy() {
	vkDestroyBuffer(device->ptr, vertexBuffer, nullptr);
	vkFreeMemory(device->ptr, vertexBufferMemory, nullptr);

	vkDestroyBuffer(device->ptr, indexBuffer, nullptr);
	vkFreeMemory(device->ptr, indexBufferMemory, nullptr);
}

void VulkanObject::draw(VkCommandBuffer buffer) {
	VkBuffer vertexBuffers[] = {vertexBuffer};
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(buffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(buffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
	vkCmdDrawIndexed(buffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}