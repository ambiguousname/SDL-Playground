#include "keyboard.h"
#include <iostream>

bool KeySource::pullData(ControlData& out) {
	if (out.type != BOOL) {
		return false;
	}

	bool pressed = SDL_GetKeyboardState(NULL)[SDL_GetScancodeFromName(keyName)] > 0;
	if (pressedDown && !pressed) {
		pressedDown = false;
		out = ControlData{BOOL, pressed};
		return true;
	} else if (pressed && !pressedDown) {
		pressedDown = true;
		out = ControlData {BOOL, pressed};
		return true;
	}
	return false;
}

bool KeyVector::pullData(ControlData& out) {
	if (out.type != VECTOR2) {
		return false;
	}

	const Uint8* keyboard = SDL_GetKeyboardState(NULL);
	out.vec = Vector2(0, 0);

	for (int i = 0; i < 4; i++) {
		bool pressed = keyboard[SDL_GetScancodeFromName(keys[i])] > 0;
		int neg = (i % 2 == 0) ? -1 : 1;
		if (i <= 1) {
			out.vec.y += neg * (pressed ? 1 : 0);
		} else {
			out.vec.x += neg * (pressed ? 1 : 0);
		}
	}

	return true;
}