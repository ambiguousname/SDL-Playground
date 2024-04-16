#pragma once
#include "controller.h"

/// @brief Source for only one key. `pullData` only returns a value when the key has just been pressed or released.
class KeySource : public ControlSource {
	protected:
	const char* keyName;
	bool pressedDown = false;
	public:
	bool isPressed() { return pressedDown; }
	KeySource(const char* keyName) {this->keyName = keyName;}
	bool pullData(ControlData& out) override;
};

class KeyVector : public ControlSource {
	protected:
	const char* keys[4];

	public:
	KeyVector(const char* keys[4]) { for (int i = 0; i < 4; i++) { this->keys[i] = keys[i]; } }
	bool pullData(ControlData& out) override;
};