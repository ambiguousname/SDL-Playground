#pragma once
#include "controller.h"

/// @brief Source for only one key. `pullData` only returns a value when the key has just been pressed or released.
class KeySource : public ControlSource {
	protected:
	std::string keyName;
	bool pressedDown = false;
	public:
	bool isPressed() { return pressedDown; }
	KeySource(std::string keyName) {this->keyName = keyName;}
	bool pullData(ControlDataOut& out) override;
};