#include "keyboard.h"
#include <iostream>

ControlDataOut KeySource::pullData(ControlDataType expected_out) {
	if (expected_out == BOOL) {
		bool pressed = SDL_GetKeyboardState(NULL)[SDL_GetScancodeFromName(keyName.c_str())] > 0;
		if (pressedDown && !pressed) {
			pressedDown = false;
			return ControlDataOut{BOOL, ControlData{pressed}};
		} else if (pressed && !pressedDown) {
			pressedDown = true;
			return ControlDataOut {BOOL, ControlData{pressed}};
		}
	}
	return ControlDataOut {NONE, false};
}