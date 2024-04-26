#include "renderer.h"
#include <algorithm>
#include "../errors.h"

void VulkanRenderer::destroy() {
	vkDestroySemaphore(device->ptr, imageAvailable, nullptr);
	vkDestroySemaphore(device->ptr, renderFinished, nullptr);
	vkDestroyFence(device->ptr, inFlight, nullptr);
	vkDestroyCommandPool(device->ptr, commandPool, nullptr);
}

void VulkanRenderer::create(VulkanSurface* surface, const VulkanLogicDevice* device) {
	this->device = device;
	this->surface = surface;
	
	
	createCommandPool();
	createSync();
}

void VulkanRenderer::createCommandPool() {
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = device->graphicsFamily;

	if (vkCreateCommandPool(device->ptr, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw AppError("Vulkan could not create a command pool.");
	}

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	// Does not need to be freed, is freed when commandPool is freed.
	if (vkAllocateCommandBuffers(device->ptr, &allocInfo, &commandBuffer) != VK_SUCCESS) {
		throw AppError("Vulkan could not allocate command buffers.");
	}
}

void VulkanRenderer::createSync() {
	VkSemaphoreCreateInfo semaphore_info{};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(device->ptr, &semaphore_info, nullptr, &imageAvailable) != VK_SUCCESS) {
		throw AppError("Vulkan could not create imageAvailable semaphore.");
	}

	if (vkCreateSemaphore(device->ptr, &semaphore_info, nullptr, &renderFinished) != VK_SUCCESS) {
		throw AppError("Vulkan could not create renderFinished semaphore.");
	}
	
	VkFenceCreateInfo fence_info{};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
 
	if (vkCreateFence(device->ptr, &fence_info, nullptr, &inFlight) != VK_SUCCESS) {
		throw AppError("Vulkan could not create inFlight fence.");
	}
}

void VulkanRenderer::draw() {
	vkWaitForFences(device->ptr, 1, &inFlight, VK_TRUE, UINT64_MAX);
	vkResetFences(device->ptr, 1, &inFlight);

	uint32_t image_index;
	VkResult result = vkAcquireNextImageKHR(device->ptr, surface->swapChain.ptr, UINT64_MAX, imageAvailable, VK_NULL_HANDLE, &image_index);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		for (auto pipeline : graphicsPipelines) {
			pipeline.refreshSwapChain();
		}
		return;
	} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw AppError("Vulkan failed to acquire swap chain");
	}
	
	
	vkResetCommandBuffer(commandBuffer, image_index);
	for (auto pipeline : graphicsPipelines) {
		pipeline.recordCommandBuffer(commandBuffer, image_index);
	}

	VkSubmitInfo submit_info{};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore wait_semaphores[] = { imageAvailable };
	VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &commandBuffer;

	VkSemaphore signal_semaphores[] = { renderFinished };
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphores;

	if (vkQueueSubmit(device->graphicsQueue, 1, &submit_info, inFlight) != VK_SUCCESS) {
		throw AppError("Vulkan could not submit command buffer to the graphics queue.");
	}
	
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signal_semaphores;

	VkSwapchainKHR swapChains[] = {surface->swapChain.ptr};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &image_index;

	vkQueuePresentKHR(device->presentQueue, &presentInfo);
}