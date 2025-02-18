#include "renderer.hpp"
#include <algorithm>
#include "../errors.hpp"

VulkanRenderPass::VulkanRenderPass(VkDevice device, const VulkanSwapChain& swapChain) {
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapChain.format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;
	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &ptr) != VK_SUCCESS) {
		throw AppError("Vulkan could not create render pass.");
	}
}

void VulkanRenderPass::destroy(VkDevice device) {
	vkDestroyRenderPass(device, ptr, nullptr);
}

void VulkanRenderer::destroy() {
	for (auto p : graphicsPipelines) {
		p.second->destroy();
		delete p.second;
	}
	renderPass.destroy(device->ptr);
	vkDestroySemaphore(device->ptr, imageAvailable, nullptr);
	vkDestroySemaphore(device->ptr, renderFinished, nullptr);
	vkDestroyFence(device->ptr, inFlight, nullptr);
	vkDestroyCommandPool(device->ptr, commandPool, nullptr);
}

VulkanRenderer::VulkanRenderer(VulkanSurface* surface, const VulkanLogicDevice* device, const VulkanPhysicalDevice* physicalDevice) : surface(surface), device(device), physicalDevice(physicalDevice) {
	renderPass = VulkanRenderPass(device->ptr, surface->swapChain);
	surface->swapChain.createFramebuffers(renderPass.ptr);
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

	// TODO: https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
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

void VulkanRenderer::refreshSwapChain() {
	vkDeviceWaitIdle(device->ptr);

	VulkanSwapChain old = surface->swapChain;

	surface->createSwapChain(old.swapChainDetails, device, old.ptr);
	surface->swapChain.createFramebuffers(renderPass.ptr);
	
	old.destroy();
}

void VulkanRenderer::recordCommandBuffers(uint32_t image_index) {
	VkCommandBufferBeginInfo begin_info{};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = 0;

	if (vkBeginCommandBuffer(commandBuffer, &begin_info) != VK_SUCCESS) {
		throw AppError("Vulkan could not start recording command buffer.");
	}

	VkRenderPassBeginInfo render_pass_info{};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_info.renderPass = renderPass.ptr;
	render_pass_info.framebuffer = surface->swapChain.framebuffers[image_index];
	render_pass_info.renderArea.offset = {0, 0};
	render_pass_info.renderArea.extent = surface->swapChain.extents;

	VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
	render_pass_info.clearValueCount = 1;
	render_pass_info.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
	for (auto pipeline : graphicsPipelines) {
		pipeline.second->recordCommandBuffer(commandBuffer, image_index);
	}
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw AppError("Vulkan could not record command buffer.");
	}
}

void VulkanRenderer::draw() {
	vkWaitForFences(device->ptr, 1, &inFlight, VK_TRUE, UINT64_MAX);
	vkResetFences(device->ptr, 1, &inFlight);

	uint32_t image_index;
	VkResult result = vkAcquireNextImageKHR(device->ptr, surface->swapChain.ptr, UINT64_MAX, imageAvailable, VK_NULL_HANDLE, &image_index);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		refreshSwapChain();
		return;
	} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw AppError("Vulkan failed to acquire swap chain");
	}
	
	vkResetCommandBuffer(commandBuffer, 0);
	recordCommandBuffers(image_index);

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

	result = vkQueuePresentKHR(device->presentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		refreshSwapChain();
	} else if (result != VK_SUCCESS) {
		throw AppError("Vulkan failed to present Swapchian.");
	}
}

void VulkanRenderer::attachPendingGraphicsPipeline(VulkanPipelineInfo* info) {
	creationInfo.push_back(info);
}

void VulkanRenderer::intializePipelines() {
	std::vector<VkGraphicsPipelineCreateInfo> creation(creationInfo.size());
	std::vector<VkPipeline> pipelinePtrs(creationInfo.size());

	for (size_t i = 0; i < creationInfo.size(); i++) {
		creation[i] = creationInfo[i]->pipelineInfo;
	}

	if (vkCreateGraphicsPipelines(device->ptr, VK_NULL_HANDLE, creation.size(), creation.data(), nullptr, pipelinePtrs.data()) != VK_SUCCESS) {
		throw AppError("Vulkan could not create graphics pipeline.");
	}

	for (size_t i = 0; i < pipelinePtrs.size(); i++) {
		VulkanPipeline* pipeline = new VulkanPipeline(pipelinePtrs[i], creationInfo[i], surface, device, physicalDevice);
		graphicsPipelines.insert(std::pair(creationInfo[i]->descriptionHash, pipeline));
		
		creationInfo[i]->destroy();
		delete creationInfo[i];
	}
	creationInfo.clear();
}