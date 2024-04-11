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

enum ControlDataType {
	BOOL,
	VECTOR2,
};

struct ControlDataOut {
	ControlDataType type;
	ControlData data;
};

/// @brief A source for `ControlData`. We translate the input we get `ControlData`.
class ControlSource {
	public:
	virtual ControlData pullData(ControlDataType expected_out) = 0;
};

/// @brief A collection of `ControlSources` so we know when and how to fire.
class Control {
	public:
	void addBinding(Event<ControlDataOut> fire_callback) { fire.subscribe(fire_callback); }
	void update();

	Control();
	
	protected:
	ControlDataType expected_out;
	std::vector<ControlSource> sources;
	EventListener<ControlDataOut> fire;
};

/// @brief A way to handle multiple input control types. For abstraction purposes, this represents ONE player and their associated controller scheme.
class Controller {
	protected:
	/// @brief The controls we hold for listening.
	std::map<std::string, Control> controls;

	public:
	Controller();
	void update();
	void listenForControl(std::string control_name, Event<ControlDataOut> listener);
	void bindControl(std::string control_name, Control to_bind);
};