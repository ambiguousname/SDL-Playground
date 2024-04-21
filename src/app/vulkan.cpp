#include "vulkan.h"
#include <SDL_vulkan.h>
#include <iostream>
#include <algorithm>
#include <set>

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

VulkanWrapper::VulkanWrapper(const App* app) : app(app) { 
	createInstance();
	createDebug();
	createSurface();
	hookDevices(); 
	createSwapChain();
}

VulkanWrapper::~VulkanWrapper() {
	vkDestroySwapchainKHR(device.ptr, swapChain.ptr, nullptr);
	vkDestroyDevice(device.ptr, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
	destroyDebug();

	vkDestroyInstance(instance, nullptr);
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
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
	
	std::vector<const char*> extensionNames(extensionCount);
	bool getNames = SDL_Vulkan_GetInstanceExtensions(app->window, &extensionCount, extensionNames.data());
	if (!getNames) {
		throw SDLError("Could not get Vulkan Instance extensions.");
	}

	if (enableValidationLayers) {
		extensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensionNames.size());
	createInfo.ppEnabledExtensionNames = extensionNames.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = validationLayers.size();
		createInfo.ppEnabledLayerNames = validationLayers.data();

		VkDebugUtilsMessengerCreateInfoEXT debugInfo = {};
        debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugInfo.pfnUserCallback = debugCallback;

		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugInfo;
	} else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}
	
	VkResult r = vkCreateInstance(&createInfo, nullptr, &instance);
	if (r != VK_SUCCESS) {
		throw AppError("Vulkan Instance Creation Failed.");
	}
}

void VulkanWrapper::createDebug() {
	if (!enableValidationLayers) { return; }

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr;

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		if (func(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
			throw AppError("Vulkan could not create debugMessenger.");
		}
	} else {
		throw AppError("Vulkan could not find the vkCreateDebugUtilsMessengerEXT function.");
	}
}

void VulkanWrapper::destroyDebug() {
	if (!enableValidationLayers) { return; }
	
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, nullptr);
	}
}

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

void VulkanWrapper::hookDevices() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw AppError("Could not find a GPU that supports Vulkan.");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (const auto& device : devices) {
		VulkanPhysicalDevice d(device, surface);
		if (d.isSuitable()) {
			physicalDevice = d;
			break;
		}
	}

	if (physicalDevice.ptr == VK_NULL_HANDLE) {
		throw AppError("Vulkan could not find a GPU that supports all requested features.");
	}

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { physicalDevice.indices.graphicsFamily.value(), physicalDevice.indices.presentFamily.value() };

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

	if (vkCreateDevice(physicalDevice.ptr, &createInfo, nullptr, &device.ptr) != VK_SUCCESS) {
		throw AppError("Vulkan could not create a logical device.");
	}
	vkGetDeviceQueue(device.ptr, physicalDevice.indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device.ptr, physicalDevice.indices.presentFamily.value(), 0, &presentQueue);
}

void VulkanWrapper::createSurface() {
	bool success = SDL_Vulkan_CreateSurface(app->window, instance, &surface);
	if (!success) {
		throw SDLError("SDL could not create a Vulkan Surface.");
	}
}

void VulkanWrapper::createSwapChain() {
	VkSurfaceFormatKHR format = physicalDevice.swapChainDetails.formats[0];
	for (const auto& aFormat : physicalDevice.swapChainDetails.formats) {
		if (aFormat.format == VK_FORMAT_B8G8R8_SRGB && aFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			format = aFormat;
		}
	}

	VkPresentModeKHR mode = VK_PRESENT_MODE_FIFO_KHR;
	for (const auto& aMode : physicalDevice.swapChainDetails.presentModes) {
		if (aMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			mode = aMode;
		}
	}

	VkExtent2D extents = physicalDevice.swapChainDetails.capabilities.currentExtent;
	if (extents.width == std::numeric_limits<uint32_t>::max()) {
		int width, height;
		SDL_GetWindowSizeInPixels(app->window, &width, &height);

		extents.width = std::clamp(static_cast<uint32_t>(width), physicalDevice.swapChainDetails.capabilities.minImageExtent.width, physicalDevice.swapChainDetails.capabilities.maxImageExtent.width);
		extents.height = std::clamp(static_cast<uint32_t>(height), physicalDevice.swapChainDetails.capabilities.minImageExtent.height, physicalDevice.swapChainDetails.capabilities.maxImageExtent.height);
	}

	uint32_t imageCount = physicalDevice.swapChainDetails.capabilities.minImageCount + 1;
	if (physicalDevice.swapChainDetails.capabilities.maxImageCount > 0 && imageCount > physicalDevice.swapChainDetails.capabilities.maxImageCount) {
		imageCount = physicalDevice.swapChainDetails.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = format.format;
	createInfo.imageColorSpace = format.colorSpace;
	createInfo.imageExtent = extents;
	// Always set to one, unless 3D stereoscopic image.
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t queueFamilyIndices[] = {physicalDevice.indices.graphicsFamily.value(), physicalDevice.indices.presentFamily.value()};

	if (physicalDevice.indices.graphicsFamily != physicalDevice.indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	} else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	// In case we want to transform the image in any way before rendering:
	createInfo.preTransform = physicalDevice.swapChainDetails.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = mode;
	createInfo.clipped = VK_TRUE;

	// TODO: This needs to allow for window resizing.
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(device.ptr, &createInfo, nullptr, &swapChain.ptr) != VK_SUCCESS) {
		throw AppError("Vulkan could not create a swap chain.");
	}


	vkGetSwapchainImagesKHR(device.ptr, swapChain.ptr, &imageCount, nullptr);
	swapChain.images.resize(imageCount);
	vkGetSwapchainImagesKHR(device.ptr, swapChain.ptr, &imageCount, swapChain.images.data());

	swapChain.format = format.format;
	swapChain.extents = extents;
}