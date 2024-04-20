#include "vulkan.h"
#include <SDL_vulkan.h>

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

bool checkValidationSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
			}
		}

	return true;
}

VulkanWrapper::~VulkanWrapper() {
	vkDestroyInstance(instance, nullptr);
}

void VulkanWrapper::createInstance() {
	if (enableValidationLayers && !checkValidationSupport()) {
		throw AppError("The requested Vulkan validation layers are not available.");
	}

	VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = app->name;
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.pEngineName = "AmbiguousEngine";
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	uint32_t extensionCount = 0;
	bool success = SDL_Vulkan_GetInstanceExtensions(app->window, &extensionCount, nullptr);
	if (!success) {
		throw SDLError("Could not get Vulkan Instance extensions.");
	}
	
	const char** extensionNames = new const char*[extensionCount];
	bool getNames = SDL_Vulkan_GetInstanceExtensions(app->window, &extensionCount, extensionNames);
	if (!getNames) {
		throw SDLError("Could not get Vulkan Instance extensions.");
	}

	createInfo.enabledExtensionCount = extensionCount;
	createInfo.ppEnabledExtensionNames = extensionNames;

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = validationLayers.size();
		createInfo.ppEnabledLayerNames = validationLayers.data();
	} else {
		createInfo.enabledLayerCount = 0;
	}
	
	VkResult r = vkCreateInstance(&createInfo, nullptr, &instance);
	if (r != VK_SUCCESS) {
		throw AppError("Vulkan Instance Creation Failed.");
	}
}

#include <iostream>
bool isDeviceSuitable(VkPhysicalDevice device) {
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	std::cout << "Found " << deviceProperties.deviceName << std::endl;

	return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;
}

void VulkanWrapper::hookDevices() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDeviceGroups(instance, &deviceCount, nullptr);
	
	if (deviceCount == 0) {
		throw AppError("Could not find a GPU that supports Vulkan.");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (const auto& device : devices) {
		if (true) {
			physicalDevice = device;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE) {
		throw AppError("Vulkan could not find a GPU that supports all requested features.");
	}
}