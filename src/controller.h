#pragma once

#include "util.h"
#include <map>
#include <string>
#include <SDL_gamecontroller.h>
#include <SDL_events.h>

/// @brief Data for `Control` struct.
union ControlData {
	SDL_GameControllerAxis axis;
	SDL_GameControllerButton button;
	SDL_KeyboardEvent key;
	/// @brief If we want to add some custom behavior.
	char* other;
};

enum ControlType {
	Axis,
	Button,
	Key,
	MousePos,
};

struct Control {
	ControlData data;
	ControlType type;
};

/// @brief A way to handle multiple input control types.
class Controller {
	/// @brief The events we hold for listening.
	std::map<std::string, EventListener<void>> controlEvents;

	public:
	void update();
	void listenForControl(std::string controlName, Event<void> listener);
	void bindControl(std::string controlName);
};