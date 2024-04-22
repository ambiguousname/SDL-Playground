#pragma once
#include "../app.h"
#include "renderer.h"
#include "devices.h"
#include <vulkan/vulkan.hpp>
#include <optional>

class App;

class VulkanWrapper {
	protected:
	const App* app;
	VkInstance instance;

	VulkanPhysicalDevice physicalDevice;
	VulkanLogicDevice device;

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
	void draw() { renderer.draw(); vkDeviceWaitIdle(device.ptr);  }
	VulkanWrapper(const App* app);
};