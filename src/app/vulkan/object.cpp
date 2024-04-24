#include "object.h"
#include "helper.h"

const std::vector<VulkanVertex> vertices = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
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
	
	VulkanHelper::copyBuffer(device, renderer->getCommandPool(), stagingBuffer, vertexBuffer, size);

	vkDestroyBuffer(device->ptr, stagingBuffer, nullptr);
	vkFreeMemory(device->ptr, stagingBufferMemory, nullptr);

	renderer->attachObject(this);
}

void VulkanObject::destroy() {
	vkDestroyBuffer(device->ptr, vertexBuffer, nullptr);
	vkFreeMemory(device->ptr, vertexBufferMemory, nullptr);
}

void VulkanObject::draw(VkCommandBuffer buffer) {
	VkBuffer vertexBuffers[] = {vertexBuffer};
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(buffer, 0, 1, vertexBuffers, offsets);
	vkCmdDraw(buffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);
}