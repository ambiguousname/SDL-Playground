#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>
#include <unordered_set>
#include <SDL3/SDL_video.h>
#include "devices.hpp"
#include "surface.hpp"
#include "helper.hpp"
#include "object.hpp"

struct VulkanRenderPass {
	VkRenderPass ptr;
	VulkanRenderPass(VkDevice device, const VulkanSwapChain& swapChain);
	VulkanRenderPass() {}
	void destroy(VkDevice device);
};

class VulkanObject;

class VulkanRenderer {
	VulkanSurface* surface;

	const VulkanLogicDevice* device;
	const VulkanPhysicalDevice* physicalDevice;
	
	VulkanRenderPass renderPass;

	// TODO: Probably a tree-based system would make things faster.
	// The things to actually render:
	std::unordered_set<VulkanObject*> vulkanObjects;

	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;

	void createCommandPool();
	void recordCommandBuffers(uint32_t image_index);

	VkSemaphore imageAvailable;
	VkSemaphore renderFinished;
	VkFence inFlight;

	void createSync();

	public:
	const VulkanLogicDevice* getDevice() const { return device; }
	const VulkanPhysicalDevice* getPhysicalDevice() const { return physicalDevice; }
	const VkCommandPool getCommandPool() const { return commandPool; }
	const VulkanRenderPass& getRenderPass() const { return renderPass; }
	VulkanSurface* getSurface() const { return surface; } 

	VulkanRenderer() {}
	
	VulkanRenderer(VulkanSurface* surface, const VulkanLogicDevice* device, const VulkanPhysicalDevice* physicalDevice);

	void attachObject(VulkanObject* o);
	void removeObject(VulkanObject* o);

	void refreshSwapChain();
	void draw();
	void destroy();
};