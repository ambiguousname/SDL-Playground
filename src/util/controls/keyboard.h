#pragma once
#include "controller.h"

class KeyboardSource : ControlSource {
	std::string keyName;
	public:
	KeyboardSource(std::string keyName) {this->keyName = keyName;}
	ControlData pullData(ControlDataType expected_out);
};