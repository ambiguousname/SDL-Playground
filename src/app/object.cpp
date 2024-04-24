#include "object.h"
#include "errors.h"
#include "vulkan/object.h"
#include "vulkan/renderer.h"

Object::Object(App& app) {
	context = app.getContext();
	renderer = app.getRenderer();

	switch (context) {
		case VULKAN:
			// Move ownership into Vulkan so it gets destroyed when Vulkan is done with it.
			inner = (void*)new VulkanObject((VulkanRenderer*)renderer);
			break;
		default:
			throw AppError("Context does not support object rendering.");
			break;
	}
}

void Object::destroy() {
	if (inner != nullptr) {
		switch (context) {
			case VULKAN:
				((VulkanRenderer*)renderer)->removeObject((VulkanObject*)inner);
			break;
			default:
				throw AppError("Context does not support object rendering. This should be unreachable, as we've reached the object destructor.");
			break;
		}
	}
}