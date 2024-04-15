#pragma once

#include "../events.h"
#include "../math.h"
#include <map>
#include <string>
#include <SDL_gamecontroller.h>
#include <SDL_events.h>
#include <memory>

/// @brief Data for `Control` struct.
union ControlData {
	bool button;
	Vector2 vec;	
};

enum ControlDataType {
	NONE,
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
	virtual ControlDataOut pullData(ControlDataType expected_out) { return ControlDataOut{NONE, false}; }
};

/// @brief A collection of `ControlSources` so we know when and how to fire.
class Control {
	public:
	void addSource(std::unique_ptr<ControlSource> source) { sources.push_back(std::move(source)); }
	void addBinding(Event<ControlDataOut> fire_callback) { fire.subscribe(fire_callback); }
	void update();

	Control(ControlDataType expected_out) { this->expected_out = expected_out; };
	
	protected:
	ControlDataType expected_out;
	std::vector<std::unique_ptr<ControlSource>> sources;
	EventListener<ControlDataOut> fire;
};

/// @brief A way to handle multiple input control types. For abstraction purposes, this represents ONE player and their associated controller scheme.
class Controller {
	protected:
	/// @brief The controls we hold for listening.
	std::map<std::string, std::unique_ptr<Control>> controls;

	public:
	Controller();
	void update();
	void listenForControl(std::string control_name, Event<ControlDataOut> listener);
	void bindControl(std::string control_name, std::unique_ptr<Control> to_bind);
};