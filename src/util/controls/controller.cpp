#include "controller.hpp"
#include <stdexcept>
#include <cstdarg>

bool ControlData::operator==(const ControlData& rhs) const {
	if (rhs.type != type) {
		return false;
	}

	switch (type) {
		case BOOL:
			return rhs.boolean == boolean;
		break;
		case VECTOR2:
			return rhs.vec == vec;
		break;
	}
	return false;
}

bool ControlData::operator!=(const ControlData& rhs) const {
	return !this->operator==(rhs);
}

// Here's my thoughts:
// Let's say you have sources set to two analog sticks.
// You're holding down one stick, but you flick the other.
// Both should be able to look at the value, and update it if theirs is different. And fire if something changes.
// That way, we are just handling everything simultaneously.
// Maybe how conflicts are resolved can also be set by an enumerator or function if we want different behaviors? But this should be fine for now.
void Control::update() {
	ControlData oldValue = value;

	for (auto i = sources.begin(); i != sources.end(); i++) {
		ControlData toChange = value;
		bool modified = i->get()->pullData(toChange);
		
		// Grab from our first source to start merging the different inputs we're getting:
		if (i == sources.begin()) {
			value = toChange;
		}

		if (modified && toChange != value) {
			switch (expected_out) {
				case VECTOR2:
					if (abs(toChange.vec.x) > abs(value.vec.x)) {
						value.vec.x = toChange.vec.x;
					}
					if (abs(toChange.vec.y) > abs(value.vec.y)) {
						value.vec.y = toChange.vec.y;
					}
					break;
				default:
					value = toChange;
					break;
			}
		}
	}

	if (value != oldValue) {
		fire.invoke(value);
	}
}

Control::Control(ControlDataType expected_out) : value{expected_out} {
	this->expected_out = expected_out;
	switch (expected_out) {
		case BOOL:
			value.boolean = false;
			break;
		case VECTOR2:
			value.vec = glm::vec2(0.f, 0.f);
			break;
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

void Controller::listenForControl(std::string control_name, Event<ControlData> listener) {
	if (auto control = controls.find(control_name); control != controls.end()) {
		control->second->addBinding(listener);
	} else {
		throw std::runtime_error("Could not get control " + control_name);
	}
}

void Controller::bindControl(std::string control_name, ControlDataType expected_out, int sources_count, ...) {
	std::shared_ptr<Control> c = std::make_shared<Control>(expected_out);

	std::va_list args;
	va_start(args, sources_count);
	for (int i = 0; i < sources_count; i++) {
		// Grab it by reference rather than copying the arg, so we keep all the functionality it needs.
		ControlSource& s = va_arg(args, ControlSource);
		ControlSource* copy = s.clone();
		c->addSource(std::unique_ptr<ControlSource>(copy));
	}
	va_end(args);

	controls.insert({control_name, std::move(c)});
}

void Controller::bindControl(std::string control_name, std::shared_ptr<Control> to_bind) {
	controls.insert({control_name, std::move(to_bind)});
}

std::shared_ptr<Control> Controller::getControl(std::string control_name) {
	if (auto control = controls.find(control_name); control != controls.end()) {
		return control->second;
	} else {
		throw std::runtime_error("Could not return control of kind " + control_name);
	}
}