#include "controller.h"
#include "../logger.h"
#include <stdexcept>
#include <cstdarg>

void Control::update() {
	for (auto i = sources.begin(); i != sources.end(); i++) {
		ControlDataOut source_data = i->get()->pullData();
		if (source_data.type == expected_out) {
			value = source_data;
			fire.invoke(source_data);
			break;
		}
	}
}

Controller::Controller() {
	static bool initialized = false;
	if (!initialized) {
		SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");
		initialized = true;
	}
}

void Controller::update() {
	for (auto c = controls.begin(); c != controls.end(); c++) {
		c->second->update();
	}
}

void Controller::listenForControl(std::string control_name, Event<ControlDataOut> listener) {
	if (auto control = controls.find(control_name); control != controls.end()) {
		control->second->addBinding(listener);
	} else {
		logErr(OTHER, "Could not get control " + control_name);
	}
}

void Controller::bindControl(std::string control_name, ControlDataType expected_out, int sources_count, ...) {
	std::shared_ptr<Control> c = std::make_shared<Control>(expected_out);

	std::va_list args;
	va_start(args, sources_count);
	for (int i = 0; i < sources_count; i++) {
		// Grab it by reference rather than copying the arg, so we keep all the functionality it needs.
		ControlSource& s = va_arg(args, ControlSource);
		c->addSource(std::unique_ptr<ControlSource>(&s));
	}

	controls.insert({control_name, std::move(c)});
}

void Controller::bindControl(std::string control_name, std::shared_ptr<Control> to_bind) {
	controls.insert({control_name, std::move(to_bind)});
}

std::shared_ptr<Control> Controller::getControl(std::string control_name) {
	if (auto control = controls.find(control_name); control != controls.end()) {
		return control->second;
	} else {
		logErr(OTHER, "Could not get control " + control_name);
		throw std::domain_error("Could not return control of kind " + control_name);
	}
}