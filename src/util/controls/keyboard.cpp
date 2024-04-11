#include "keyboard.h"

ControlData KeyboardSource::pullData(ControlDataType expected_out) {
	if (expected_out == BOOL) {
		SDL_GetKeyboardState(NULL)[SDL_GetScancodeFromName(keyName.c_str())];
	}
}