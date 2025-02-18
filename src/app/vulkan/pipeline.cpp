#include "pipeline.hpp"
#include "../errors.hpp"

VulkanPipelineInfo::VulkanPipelineInfo(VulkanRenderer* renderer, ShaderDescription description) : description(description) , descriptionHash(description.descriptionHash) {
	inputAssembly = VkPipelineInputAssemblyStateCreateInfo{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VulkanSurface* surface = renderer->getSurface();
	this->device = renderer->getDevice();

	viewport = VkViewport {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float) surface->swapChain.extents.width;
	viewport.height = (float) surface->swapChain.extents.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	scissor = VkRect2D{};
	scissor.offset = {0, 0};
	scissor.extent = surface->swapChain.extents;

	dynamicStates = std::vector<VkDynamicState>{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	dynamicState = VkPipelineDynamicStateCreateInfo{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	viewportState = VkPipelineViewportStateCreateInfo{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	rasterizer = VkPipelineRasterizationStateCreateInfo{};
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

	multisampling = VkPipelineMultisampleStateCreateInfo{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	colorBlendAttachment = VkPipelineColorBlendAttachmentState{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	colorBlending = VkPipelineColorBlendStateCreateInfo{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	// TODO: Make more flexible.
	VkDescriptorSetLayoutBinding projectionLayoutBinding{};
	projectionLayoutBinding.binding = 0;
	projectionLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	projectionLayoutBinding.descriptorCount = 1;
	projectionLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	projectionLayoutBinding.pImmutableSamplers = nullptr;

	
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &projectionLayoutBinding;

	if (vkCreateDescriptorSetLayout(device->ptr, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw AppError("Vulkan could not create a Descriptor Set Layout for shader.");
	}

	pipelineLayoutInfo = VkPipelineLayoutCreateInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	if (vkCreatePipelineLayout(device->ptr, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw AppError("Vulkan could not create a render pipeline layout.");
	}

	pipelineInfo = VkGraphicsPipelineCreateInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = this->description.shaderStages.size();
	pipelineInfo.pStages = this->description.shaderStages.data();
	pipelineInfo.pVertexInputState = &this->description.vertexInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr; // Optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderer->getRenderPass().ptr;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional
}

void VulkanPipelineInfo::destroy() {
	description.destroy();
}

VulkanPipeline::VulkanPipeline(VkPipeline ptr, VulkanPipelineInfo* creationInfo, VulkanSurface* surface, const VulkanLogicDevice* device, const VulkanPhysicalDevice* physicalDevice) : ptr(ptr), pipelineLayout(creationInfo->pipelineLayout), descriptorSetLayout(creationInfo->descriptorSetLayout), surface(surface), device(device) {
	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	// TODO: https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
	poolSize.descriptorCount = 1;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	// TODO: https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
	poolInfo.maxSets = 1;

	if (vkCreateDescriptorPool(device->ptr, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}

	// TODO: https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
	std::vector<VkDescriptorSetLayout> layouts(1, descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	// TODO: https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = layouts.data();

	// TODO: https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
	descriptorSets.resize(1);
	if (vkAllocateDescriptorSets(device->ptr, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	
	// TODO: https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
	// FIXME: Does this belong in the object?
	// Uniform buffers:
	VkDeviceSize bufferSize = sizeof(DisplayMatrices);
	uniformBuffers.resize(1);
	uniformBuffersMemory.resize(1);
	uniformBuffersMapped.resize(1);

	for (size_t i = 0; i < 1; i++) {
		VulkanHelper::createBuffer(device->ptr, physicalDevice, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);

		vkMapMemory(device->ptr, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);

		VkDescriptorBufferInfo bufferInfo{};
		// TODO: https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
		bufferInfo.buffer = uniformBuffers[0];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(DisplayMatrices);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		// TODO: https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
		descriptorWrite.dstSet = descriptorSets[0];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = nullptr; // Optional
		descriptorWrite.pTexelBufferView = nullptr; // Optional
		vkUpdateDescriptorSets(device->ptr, 1, &descriptorWrite, 0, nullptr);
	}
}

void VulkanPipeline::destroy() {
	for (auto o : objects) {
		o->destroy();
	}
	objects.clear();

	vkDestroyPipelineLayout(device->ptr, pipelineLayout, nullptr);
	vkDestroyPipeline(device->ptr, ptr, nullptr);

	// TODO: https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
	for (size_t i = 0; i < 1; i++) {
        vkDestroyBuffer(device->ptr, uniformBuffers[i], nullptr);
        vkFreeMemory(device->ptr, uniformBuffersMemory[i], nullptr);
    }

	vkDestroyDescriptorPool(device->ptr, descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(device->ptr, descriptorSetLayout, nullptr);
}

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
void VulkanPipeline::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t image_index) {
	if (objects.size() < 0) {
		return;
	}

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, ptr);
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(surface->swapChain.extents.width);
	viewport.height = static_cast<float>(surface->swapChain.extents.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = surface->swapChain.extents;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	
	// TODO: Make this flexible.
	DisplayMatrices test{};
	test.model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	test.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	test.proj = glm::perspective(glm::radians(45.0f), 0.5f, 0.1f, 10.0f);
	// test.proj[1][1] *= -1;

	// TODO: https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
	memcpy(uniformBuffersMapped[0], &test, sizeof(test));

	// TODO: https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[0], 0, nullptr);

	for (auto o : objects) {
		o->draw(commandBuffer);
	}

	vkCmdEndRenderPass(commandBuffer);
}



void VulkanPipeline::attachObject(VulkanObject* o) {
	objects.insert(o);
}

void VulkanPipeline::removeObject(VulkanObject* o) { 
	objects.erase(o);
}