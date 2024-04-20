#pragma once
#include "app.h"
#include <vulkan/vulkan.hpp>

class App;

class VulkanWrapper {
	protected:
	const App* app;
	VkInstance instance;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

	void createInstance();
	void hookDevices();

	public:
	~VulkanWrapper();
	VulkanWrapper(const App* app) : app(app) { createInstance(); /*hookDevices();*/ }
};