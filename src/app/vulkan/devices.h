#pragma once
#include <optional>
#include <vulkan/vulkan.hpp>


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
	VkPhysicalDevice ptr = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures features;
	VkPhysicalDeviceMemoryProperties memoryProperties;
	QueueFamilyIndices indices;
	SwapChainSupportDetails swapChainDetails;

	VulkanPhysicalDevice(VkPhysicalDevice device, VkSurfaceKHR surface);
	VulkanPhysicalDevice() { }
	bool isSuitable();
};

struct VulkanLogicDevice {
	VkDevice ptr = VK_NULL_HANDLE;

	uint32_t graphicsFamily;
	uint32_t presentFamily;

	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VulkanLogicDevice(const VulkanPhysicalDevice& physicalDevice, bool enableValidationLayers, const std::vector<const char*>& validationLayers);
	VulkanLogicDevice() {}
	void destroy();
};