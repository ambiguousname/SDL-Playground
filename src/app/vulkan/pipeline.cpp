#include "pipeline.h"
#include "../errors.h"

void VulkanPipeline::destroy() {
	for (auto o : objects) {
		o->destroy();
		delete o;
	}
	objects.clear();

	vkDestroyPipelineLayout(device->ptr, pipelineLayout, nullptr);
	vkDestroyPipeline(device->ptr, ptr, nullptr);
}

void VulkanPipeline::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t image_index) {
	if (objects.size() < 0) {
		return;
	}

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, ptr);
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(surface->swapChain.extents.width);
	viewport.height = static_cast<float>(surface->swapChain.extents.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = surface->swapChain.extents;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	for (auto o : objects) {
		o->draw(commandBuffer);
	}

	vkCmdEndRenderPass(commandBuffer);
}



void VulkanPipeline::attachObject(VulkanObject* o) {
	objects.insert(o);
}

void VulkanPipeline::removeObject(VulkanObject* o) { 
	objects.erase(o);
	delete o; 
}