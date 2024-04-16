#include "controller.h"
#include "../logger.h"
#include <stdexcept>

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

void Controller::bindControl(std::string control_name, std::shared_ptr<Control> to_bind) {
	controls.insert({control_name, std::move(to_bind)});
}

std::weak_ptr<Control> Controller::getControl(std::string control_name) {
	if (auto control = controls.find(control_name); control != controls.end()) {
		return std::weak_ptr<Control>(control->second);
	} else {
		logErr(OTHER, "Could not get control " + control_name);
		throw std::domain_error("Could not return control of kind " + control_name);
	}
}