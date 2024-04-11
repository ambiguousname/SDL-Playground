#include "controller.h"
#include "../logger.h"

void Control::update() {
	for (auto source : sources) {
		
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
	for (auto control : controls) {
		control.second.update();
	}
}

void Controller::listenForControl(std::string control_name, Event<ControlDataOut> listener) {
	if (auto control = controls.find(control_name); control != controls.end()) {
		control->second.addBinding(listener);
	} else {
		logErr(OTHER, "Could not get control " + control_name);
	}
}

void Controller::bindControl(std::string control_name, Control to_bind) {
	controls[control_name] = to_bind;
} 