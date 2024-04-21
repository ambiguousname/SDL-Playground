#pragma once
#include "app.h"
#include <vulkan/vulkan.h>
#include <optional>

class App;


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

struct VulkanDevice {
	VkDevice ptr;
	VulkanDevice() { ptr = VK_NULL_HANDLE; }
};

struct VulkanSwapChain {
	VkSwapchainKHR ptr;
	std::vector<VkImage> images;
	std::vector<VkImageView> imageViews;
	VkFormat format;
	VkExtent2D extents;
};

class VulkanWrapper {
	protected:
	const App* app;
	VkInstance instance;

	VulkanPhysicalDevice physicalDevice;
	VulkanDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkSurfaceKHR surface;
	VulkanSwapChain swapChain;

	VkDebugUtilsMessengerEXT debugMessenger;

	void createInstance();
	void hookDevices();
	void createSurface();
	void createSwapChain();

	
	void createDebug();
	void destroyDebug();

	public:
	~VulkanWrapper();
	VulkanWrapper(const App* app);
};