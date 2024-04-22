#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>
#include <SDL_video.h>
#include <optional>


struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;
	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct VulkanPhysicalDevice {
	VkPhysicalDevice ptr;
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures features;
	QueueFamilyIndices indices;
	SwapChainSupportDetails swapChainDetails;
	VulkanPhysicalDevice(VkPhysicalDevice device, VkSurfaceKHR surface);
	VulkanPhysicalDevice() { ptr = VK_NULL_HANDLE; }
	bool isSuitable();
};

struct VulkanSwapChain {
	VkSwapchainKHR ptr;
	std::vector<VkImage> images;
	std::vector<VkImageView> imageViews;
	std::vector<VkFramebuffer> framebuffers;
	VkDevice device;
	VkFormat format;
	VkExtent2D extents;

	void createFramebuffers(VkRenderPass renderPass);
	void createImageViews();
	void destroy();
	VulkanSwapChain(SDL_Window* window, VkSurfaceKHR surface, const VulkanPhysicalDevice& physicalDevice, VkDevice device);
	VulkanSwapChain() {}
};

struct VulkanRenderPass {
	VkRenderPass ptr;
	VulkanRenderPass(VkDevice device, const VulkanSwapChain& swapChain);
	VulkanRenderPass() {}
	void destroy(VkDevice device);
};

class VulkanRenderer {
	VulkanSwapChain swapChain;
	VkDevice device;
	VulkanRenderPass renderPass;
	
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;

	void createCommandPool(const VulkanPhysicalDevice& physicalDevice);

	VkSemaphore imageAvailable;
	VkSemaphore renderFinished;
	VkFence inFlight;

	void createSync();

	public:
	void destroy();
	void draw(VkQueue graphicsQueue, VkQueue presentQueue);
	VulkanRenderer() {}
	void recordCommandBuffer(uint32_t image_index);
	VulkanRenderer(SDL_Window* window, VkSurfaceKHR surface, const VulkanPhysicalDevice& physicalDevice, VkDevice device);
};