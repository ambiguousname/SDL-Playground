#pragma once

#include "../events.h"
#include "../math.h"
#include <map>
#include <string>
#include <SDL_gamecontroller.h>
#include <SDL_events.h>
#include <memory>

/// @brief Data that `ControlSource` returns.
union ControlData {
	bool boolean;
	Vector2 vec;	
};

/// @brief The type of data we've just returned.
enum ControlDataType {
	/// @brief There is no associated data. Ignore this value.
	NONE,
	BOOL,
	VECTOR2,
};

struct ControlDataOut {
	ControlDataType type;
	ControlData data;
};

/// @brief A source of control data. This can be a specific key, a controller axis, a button, whatever you can think of.
/// We configure this source before passing it off to `Control` to use.
class ControlSource {
	public:
	virtual ControlDataOut pullData() { return ControlDataOut{NONE, false}; }
};

/// @brief A specific kind of input, determined by `expected_out`. We could say that we're expecting a boolean, or a vector2, or whatever. The main idea is that we're going to configure our sources to produce that result however we want.
class Control {
	protected:
	ControlDataType expected_out;
	std::vector<std::unique_ptr<ControlSource>> sources;
	EventListener<ControlDataOut> fire;
	ControlDataOut value;
	void addBinding(Event<ControlDataOut> fire_callback) { fire.subscribe(fire_callback); }

	friend class Controller;

	public:
	void addSource(std::unique_ptr<ControlSource> source) { sources.push_back(std::move(source)); }
	void update();
	ControlDataOut getValue() { return value; }

	Control(ControlDataType expected_out) { this->expected_out = expected_out; };
};

/// @brief A way to handle multiple input control types. For abstraction purposes, this represents ONE player and their associated controller scheme.
class Controller {
	protected:
	/// @brief The controls we hold for listening.
	std::map<std::string, std::shared_ptr<Control>> controls;

	public:
	Controller();
	void update();
	void listenForControl(std::string control_name, Event<ControlDataOut> listener);
	void bindControl(std::string control_name, std::shared_ptr<Control> to_bind);
	std::weak_ptr<Control> getControl(std::string control_name);
};