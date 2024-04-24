#include "object.h"

const std::vector<VulkanVertex> vertices = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};

VulkanObject::VulkanObject(VkDevice device, VkPhysicalDeviceMemoryProperties memoryProperties) {
	this->device = device;

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeof(vertices[0]) * vertices.size();
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferInfo.flags = 0;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
		throw AppError("Vulkan could not create vertex buffer.");
	}
	
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, vertexBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;

	uint32_t type = 0;
	bool typeFound = false;
	uint32_t filter = (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
		if ((memRequirements.memoryTypeBits & (i << 1)) && (filter & memoryProperties.memoryTypes[i].propertyFlags) ==  filter) {
			type = i;
			typeFound = true;
			break;
		}
	}
	if (!typeFound) {
		throw AppError("Could not find memory type to allocate object vertex buffer.");
	}

	allocInfo.memoryTypeIndex = type;

	if (vkAllocateMemory(device, &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS) {
		throw AppError("Vulkan could not allocate memory for object vertex buffer.");
	}

	vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);

	void* data;
	vkMapMemory(device, vertexBufferMemory, 0, bufferInfo.size, 0, &data);
	memcpy(data, vertices.data(), (size_t) bufferInfo.size);
	vkUnmapMemory(device, vertexBufferMemory);
}

void VulkanObject::destroy() {
	vkDestroyBuffer(device, vertexBuffer, nullptr);
	vkFreeMemory(device, vertexBufferMemory, nullptr);
}

void VulkanObject::draw(VkCommandBuffer buffer) {
	VkBuffer vertexBuffers[] = {vertexBuffer};
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(buffer, 0, 1, vertexBuffers, offsets);
	vkCmdDraw(buffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);
}