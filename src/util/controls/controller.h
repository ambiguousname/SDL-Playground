#pragma once

#include "../events.h"
#include "../math.h"
#include <map>
#include <string>
#include <SDL_gamecontroller.h>
#include <SDL_events.h>

/// @brief Data for `Control` struct.
union ControlData {
	bool button;
	Vector2 vec;	
};

/// @brief Our data source
union ControlDataSource {
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
	Other,
};

class Control {
	ControlData data;
	ControlType type;
	
	protected:
	ControlDataSource source;
};

/// @brief A way to handle multiple input control types.
class Controller {
	protected:
	/// @brief The events we hold for listening.
	std::map<std::string, EventListener<Control>> controlEvents;

	public:
	void update();
	void updateKeyboard(SDL_Event e);
	void listenForControl(std::string controlName, Event<Control> listener);
	void bindControl(std::string controlName);
};