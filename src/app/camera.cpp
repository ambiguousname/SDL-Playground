#include "camera.hpp"
#include "vulkan/camera.hpp"

Camera::Camera(App& app) {
	context = app.getContext();
	renderer = app.getRenderer();
	if (context == VULKAN) {
		VulkanRenderer* r = (VulkanRenderer*)renderer;

		VulkanSwapChain swapChain = r->getSurface()->swapChain;

		VulkanCamera* cam = new VulkanCamera(r, swapChain.extents.width, swapChain.extents.height);

		inner = (void*)cam;
	}
}

void Camera::update() {
	if(context == VULKAN) {
		((VulkanCamera*)inner)->draw();
	}
}

void Camera::destroy() {
	if (context == VULKAN) {
		VulkanCamera* c = (VulkanCamera*) inner;
		c->destroy();
		delete c;
	}
}