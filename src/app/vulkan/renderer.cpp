#include "renderer.h"
#include <algorithm>
#include "../app.h"
#include "shader.h"

void VulkanSwapChain::destroy() {
	for (auto imageView : imageViews) {
		vkDestroyImageView(device, imageView, nullptr);
	}
	for (auto framebuffer : framebuffers) {
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}
	vkDestroySwapchainKHR(device, ptr, nullptr);
}

void VulkanSwapChain::createImageViews() {
	imageViews.resize(images.size());
	for (size_t i = 0; i < images.size(); i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = images[i];

		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = format;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		if (vkCreateImageView(device, &createInfo, nullptr, &imageViews[i]) != VK_SUCCESS) {
			throw AppError("Vulkan failed to create image views.");
		}
	}
}

VulkanSwapChain::VulkanSwapChain(SDL_Window* window, VkSurfaceKHR surface, const VulkanPhysicalDevice& physicalDevice, VkDevice device) {
	this->device = device;
	VkSurfaceFormatKHR format = physicalDevice.swapChainDetails.formats[0];
	for (const auto& aFormat : physicalDevice.swapChainDetails.formats) {
		if (aFormat.format == VK_FORMAT_B8G8R8_SRGB && aFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			format = aFormat;
		}
	}
	
	this->format = format.format;

	VkPresentModeKHR mode = VK_PRESENT_MODE_FIFO_KHR;
	for (const auto& aMode : physicalDevice.swapChainDetails.presentModes) {
		if (aMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			mode = aMode;
		}
	}

	this->extents = physicalDevice.swapChainDetails.capabilities.currentExtent;
	if (extents.width == std::numeric_limits<uint32_t>::max()) {
		int width, height;
		SDL_GetWindowSizeInPixels(window, &width, &height);

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
	createInfo.imageExtent = VkExtent2D(extents);
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

	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &ptr) != VK_SUCCESS) {
		throw AppError("Vulkan could not create a swap chain.");
	}


	vkGetSwapchainImagesKHR(device, ptr, &imageCount, nullptr);
	images.resize(imageCount);
	vkGetSwapchainImagesKHR(device, ptr, &imageCount, images.data());

	createImageViews();
}

VulkanRenderPass::VulkanRenderPass(VkDevice device, const VulkanSwapChain& swapChain) {
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapChain.format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &ptr) != VK_SUCCESS) {
		throw AppError("Vulkan could not create render pass.");
	}
}

void VulkanRenderPass::destroy(VkDevice device) {
	vkDestroyRenderPass(device, ptr, nullptr);
}

void VulkanRenderer::destroy() {
	swapChain.destroy();
	renderPass.destroy(this->device);
	vkDestroyPipelineLayout(this->device, pipelineLayout, nullptr);
	vkDestroyPipeline(this->device, graphicsPipeline, nullptr);
}

VulkanRenderer::VulkanRenderer(SDL_Window* window, VkSurfaceKHR surface, const VulkanPhysicalDevice& physicalDevice, VkDevice device) {
	this->device = device;

	swapChain = VulkanSwapChain(window, surface, physicalDevice, device);
	renderPass = VulkanRenderPass(this->device, swapChain);

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;	

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float) swapChain.extents.width;
	viewport.height = (float) swapChain.extents.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = VkExtent2D(swapChain.extents);

	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;

	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0; // Optional
	pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw AppError("Vulkan could not create a render pipeline layout.");
	}

	VulkanShader vert(device, "shaders/vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
	VulkanShader frag(device, "shaders/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
	VkPipelineShaderStageCreateInfo shaderStages[] = {vert.info, frag.info};

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr; // Optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass.ptr;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		throw AppError("Vulkan could not create graphics pipeline.");
	}

	vkDestroyShaderModule(device, frag.shaderModule, nullptr);
	vkDestroyShaderModule(device, vert.shaderModule, nullptr);

	swapChain.createFramebuffers(renderPass.ptr);
}

void VulkanSwapChain::createFramebuffers(VkRenderPass renderPass) {
	framebuffers.resize(imageViews.size());

	for (size_t i = 0; i < imageViews.size(); i++) {
		VkImageView attachments[] = {
			imageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = extents.width;
		framebufferInfo.height = extents.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
			throw AppError("Vulkan could not create a framebuffer.");
		}
	}
}