#pragma once
#include "../app.h"
#include "renderer.h"
#include <vulkan/vulkan.h>
#include <optional>

class App;

class VulkanWrapper {
	protected:
	const App* app;
	VkInstance instance;

	VulkanPhysicalDevice physicalDevice;
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkSurfaceKHR surface;
	VulkanRenderer renderer;

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