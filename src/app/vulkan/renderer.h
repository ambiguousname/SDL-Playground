#pragma once
#include <vulkan/vulkan.h>
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
	VkFormat format;
	VkExtent2D extents;

	void createImageViews(VkDevice device);
	void destroy(VkDevice device);
	VulkanSwapChain(SDL_Window* window, VkSurfaceKHR surface, const VulkanPhysicalDevice& physicalDevice, VkDevice device);
	VulkanSwapChain() {}
};

struct VulkanRenderPass {
	VkRenderPass ptr;
	VulkanRenderPass(VkDevice device, const VulkanSwapChain& swapChain);
	void destroy(VkDevice device);
};

class VulkanRenderer {
	VulkanSwapChain swapChain;
	VkPipelineLayout pipelineLayout;
	VkDevice device;
	std::vector<VulkanRenderPass> passes;

	public:
	void destroy();
	VulkanRenderer() {}
	VulkanRenderer(SDL_Window* window, VkSurfaceKHR surface, const VulkanPhysicalDevice& physicalDevice, VkDevice device);
};