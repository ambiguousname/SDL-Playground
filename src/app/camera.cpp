#include "camera.hpp"
#include "vulkan/camera.hpp"

Camera::Camera(App& app) : Object(app) {
	if (context == VULKAN) {
		VulkanRenderer* r = (VulkanRenderer*)renderer;

		VulkanSwapChain swapChain = r->getSurface()->swapChain;

		VulkanCamera* cam = new VulkanCamera(r, swapChain.extents.width, swapChain.extents.height);

		inner = (void*)cam;
		
		r->attachObject(cam);
	}
}

void Camera::_destroy() {
	if (context == VULKAN) {
		VulkanCamera* c = (VulkanCamera*) inner;
		((VulkanRenderer*)renderer)->removeObject(c);
		c->destroy();
		delete c;
	}
}