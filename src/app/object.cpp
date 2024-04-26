#include "object.h"
#include "errors.h"
#include "vulkan/object.h"
#include "vulkan/renderer.h"

Object::Object(App& app) {
	context = app.getContext();
	renderer = app.getRenderer();

	if (context == VULKAN) {
		VulkanRenderer* r = ((VulkanRenderer*)renderer);
		const VulkanLogicDevice* device = r->getDevice();
		VkCommandPool commandPool = r->getCommandPool();
		
		VulkanShader<VulkanVertex> vert(device->ptr, "shaders/vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
		VulkanShader<VulkanVertex> frag(device->ptr, "shaders/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
		ShaderDescription<VulkanVertex> description({vert, frag});

		// TODO: Don't make multiple pipelines per object if you can help it.
		// The only thing that varies is the shader description generic, so test based on that?
		VulkanPipeline* p = new VulkanPipeline(r, description.getShaderStages(), description.vertexInfo);
		((VulkanRenderer*)renderer)->attachPipeline(p);

		description.destroy();
		// Move ownership into Vulkan so it gets destroyed when Vulkan is done with it.
		inner = (void*)new VulkanObject(p, device, ((VulkanRenderer*)renderer)->getPhysicalDevice(), ((VulkanRenderer*)renderer)->getCommandPool());
	} else {
		throw AppError("Context does not support object rendering.");
	}
}

void Object::destroy() {
	if (inner != nullptr) {
		switch (context) {
			case VULKAN:
				((VulkanObject*)inner)->parentPipeline->removeObject((VulkanObject*)inner);
			break;
			default:
				throw AppError("Context does not support object rendering. This should be unreachable, as we've reached the object destructor.");
			break;
		}
	}
}