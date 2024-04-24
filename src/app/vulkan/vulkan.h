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

	VulkanSurface surface;
	VulkanRenderer renderer;

	VkDebugUtilsMessengerEXT debugMessenger;

	void createInstance();
	void createDebug();
	void hookDevices();
	
	void destroyDebug();

	public:
	const VulkanLogicDevice* getDevice() const { return &device; }
	VulkanRenderer* getRenderer() { return &renderer; }

	void destroy();
	void draw() { renderer.draw(); vkDeviceWaitIdle(device.ptr);  }
	VulkanWrapper(const App* app);
};