#include "object.hpp"
#include "errors.hpp"
#include "vulkan/object.hpp"
#include "vulkan/renderer.hpp"

Object::Object(App& app) {
	context = app.getContext();
	renderer = app.getRenderer();

	this->_create();

	isActive = true;
}

void Object::update() {
	if (!isActive) {
		return;
	}
	this->_update();
}


void Object::destroy() {
	if (!isActive) {
		return;
	}
	this->_destroy();
	
	inner = nullptr;
	renderer = nullptr;

	isActive = false;
}