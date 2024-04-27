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
		// FIXME: Should be able to select what pipeline you want during object creation.
		VulkanPipeline* p = r->getPipeline(typeid(VulkanVertex).hash_code()); 
		// Move ownership into Vulkan so it gets destroyed when Vulkan is done with it.
		inner = (void*)new VulkanObject(device, ((VulkanRenderer*)renderer)->getPhysicalDevice(), ((VulkanRenderer*)renderer)->getCommandPool());
		p->attachObject((VulkanObject*)inner);
	} else {
		throw AppError("Context does not support object rendering.");
	}
}

void Object::destroy() {
	if (inner != nullptr) {
		switch (context) {
			case VULKAN:
				((VulkanRenderer*)renderer)->getPipeline(typeid(VulkanVertex).hash_code())->removeObject((VulkanObject*)inner);
			break;
			default:
				throw AppError("Context does not support object rendering. This should be unreachable, as we've reached the object destructor.");
			break;
		}
	}
}