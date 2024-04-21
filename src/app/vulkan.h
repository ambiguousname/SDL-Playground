#pragma once
#include "app.h"
#include <vulkan/vulkan.hpp>
#include <optional>

class App;


struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	bool isComplete() {
		return graphicsFamily.has_value();
	}
};

struct VulkanPhysicalDevice {
	VkPhysicalDevice ptr;
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures features;
	QueueFamilyIndices indices;
	VulkanPhysicalDevice(VkPhysicalDevice device);
	VulkanPhysicalDevice() { ptr = VK_NULL_HANDLE; }
	bool isSuitable();
};

struct VulkanDevice {
	VkDevice ptr;
	VulkanDevice() { ptr = VK_NULL_HANDLE; }
};

class VulkanWrapper {
	protected:
	const App* app;
	VkInstance instance;

	VulkanPhysicalDevice physicalDevice;
	VulkanDevice device;
	VkQueue graphicsQueue;

	VkDebugUtilsMessengerEXT debugMessenger;

	void createInstance();
	void hookDevices();
	void createSurface();

	
	void createDebug();
	void destroyDebug();

	public:
	~VulkanWrapper();
	VulkanWrapper(const App* app);
};