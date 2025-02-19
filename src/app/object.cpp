#include "object.hpp"
#include "errors.hpp"
#include "vulkan/object.hpp"
#include "vulkan/renderer.hpp"

Object::Object(App& app) {
	context = app.getContext();
	renderer = app.getRenderer();

	if (context == VULKAN) {
		VulkanRenderer* r = ((VulkanRenderer*)renderer);

		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(VulkanVertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(VulkanVertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(VulkanVertex, color);

		// Default pipeline:
		VulkanShader vert(r->getDevice()->ptr, "shaders/main.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
		VulkanShader frag(r->getDevice()->ptr, "shaders/main.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

		ShaderDescription description = ShaderDescription(bindingDescriptions, attributeDescriptions,  std::vector<VulkanShader>({vert, frag}));

		VulkanPipelineInfo n = VulkanPipelineInfo(r, description);
		
		// Move ownership into Vulkan so it gets destroyed when Vulkan is done with it.
		inner = (void*)new VulkanObject(r->getDevice(), r->getPhysicalDevice(), r->getSurface(), n, r->getCommandPool());
		
		r->attachObject((VulkanObject*)inner);
	} else {
		throw AppError("Context does not support object rendering.");
	}
}

void Object::destroy() {
	if (inner != nullptr) {
		switch (context) {
			case VULKAN:
				((VulkanRenderer*)renderer)->removeObject((VulkanObject*)inner);
				delete inner;
			break;
			default:
				throw AppError("Context does not support object rendering. This should be unreachable, as we've reached the object destructor.");
			break;
		}
	}
}