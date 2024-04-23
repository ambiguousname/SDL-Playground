#include "devices.h"
#include "../errors.h"
#include <iostream>
#include <set>

VulkanPhysicalDevice::VulkanPhysicalDevice(VkPhysicalDevice device, VkSurfaceKHR surface) {
	ptr = device;
	vkGetPhysicalDeviceProperties(device, &properties);
	vkGetPhysicalDeviceFeatures(device, &features);

	std::cout << "Found " << properties.deviceName << std::endl;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);


	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
		if (presentSupport) {
			indices.presentFamily = true;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &swapChainDetails.capabilities);
	
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount != 0){
		swapChainDetails.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, swapChainDetails.formats.data());
	}
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		swapChainDetails.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, swapChainDetails.presentModes.data());
	}
}


const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

bool VulkanPhysicalDevice::isSuitable() {
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(ptr, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(ptr, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	bool extensionsSupported = requiredExtensions.empty();

	bool swapChainAdequate = false;
	if (extensionsSupported) {
		swapChainAdequate = !swapChainDetails.formats.empty() && !swapChainDetails.presentModes.empty();
	}

	return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

VulkanLogicDevice::VulkanLogicDevice(const VulkanPhysicalDevice& physicalDevice, bool enableValidationLayers, const std::vector<const char*>& validationLayers) {
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

	graphicsFamily = physicalDevice.indices.graphicsFamily.value();
	presentFamily = physicalDevice.indices.presentFamily.value();

	std::set<uint32_t> uniqueQueueFamilies = { graphicsFamily, presentFamily };

	float priority = 1.0f;
	for (auto family : uniqueQueueFamilies ) {
		VkDeviceQueueCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		info.queueFamilyIndex = family;
		info.queueCount = 1;
		info.pQueuePriorities = &priority;
		queueCreateInfos.push_back(info);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	} else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice.ptr, &createInfo, nullptr, &ptr) != VK_SUCCESS) {
		throw AppError("Vulkan could not create a logical device.");
	}
	
	vkGetDeviceQueue(ptr, graphicsFamily, 0, &graphicsQueue);
	vkGetDeviceQueue(ptr, presentFamily, 0, &presentQueue);
}

void VulkanLogicDevice::destroy() {
	vkDestroyDevice(ptr, nullptr);
}