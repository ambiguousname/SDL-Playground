#pragma once
#include "controller.h"

/// @brief Source for only one key. `pullData` only returns a value when the key has just been pressed or released.
class KeySource : public ControlSource {
	std::string keyName;
	bool pressedDown = false;
	public:
	KeySource(std::string keyName) {this->keyName = keyName;}
	ControlDataOut pullData(ControlDataType expected_out) override;
};