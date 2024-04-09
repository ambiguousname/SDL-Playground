#include "controller.h"
#include "../logger.h"

void Controller::updateKeyboard(SDL_Event e) {
	switch (e.type) {
		case SDL_KEYDOWN:

		break;
		case SDL_KEYUP:
		break;
	}
}

void Controller::update() {
}

void Controller::listenForControl(std::string controlName, Event<Control> listener) {
	if (auto control = controlEvents.find(controlName); control != controlEvents.end()) {
		control->second.subscribe(listener);
	} else {
		logErr(OTHER, "Could not get control " + controlName);
	}
}

void Controller::bindControl(std::string controlName) {
	
}