#include "helper.hpp"
#include "../errors.hpp"

void VulkanHelper::createBuffer(VkDevice device, const VulkanPhysicalDevice* physicalDevice, VkDeviceSize bufferSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = bufferSize;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferInfo.flags = 0;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw AppError("Vulkan could not create vertex buffer.");
	}
	
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;

	uint32_t type = 0;
	bool typeFound = false;
	
	for (uint32_t i = 0; i < physicalDevice->memoryProperties.memoryTypeCount; i++) {
		if ((memRequirements.memoryTypeBits & (i << 1)) && (properties & physicalDevice->memoryProperties.memoryTypes[i].propertyFlags) ==  properties) {
			type = i;
			typeFound = true;
			break;
		}
	}
	if (!typeFound) {
		throw AppError("Could not find memory type to allocate object vertex buffer.");
	}

	allocInfo.memoryTypeIndex = type;

	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw AppError("Vulkan could not allocate memory for object vertex buffer.");
	}

	vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void VulkanHelper::copyBuffer(const VulkanLogicDevice* device, VkCommandPool commandPool, VkBuffer src, VkBuffer dst, VkDeviceSize size) {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer copyBuffer;
	vkAllocateCommandBuffers(device->ptr, &allocInfo, &copyBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(copyBuffer, &beginInfo);
	
	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;
	vkCmdCopyBuffer(copyBuffer, src, dst, 1, &copyRegion);

	vkEndCommandBuffer(copyBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &copyBuffer;

	vkQueueSubmit(device->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(device->graphicsQueue);

	vkFreeCommandBuffers(device->ptr, commandPool, 1, &copyBuffer);
}