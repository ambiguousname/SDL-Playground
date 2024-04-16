#include "keyboard.h"
#include <iostream>

bool KeySource::pullData(ControlDataOut& out) {
	if (out.type != BOOL) {
		return false;
	}
	
	bool pressed = SDL_GetKeyboardState(NULL)[SDL_GetScancodeFromName(keyName.c_str())] > 0;
	if (pressedDown && !pressed && out.data.boolean == true) {
		pressedDown = false;
		out = ControlDataOut{BOOL, ControlData{pressed}};
		return true;
	} else if (pressed && !pressedDown && out.data.boolean == false) {
		pressedDown = true;
		out = ControlDataOut {BOOL, ControlData{pressed}};
		return true;
	}
	return false;
}