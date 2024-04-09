#include "logger.h"
#include <iostream>
#include <SDL.h>

void logErr(ErrorKind kind, std::string message) {
	if (kind == SDL) {
		std::cerr << message << " Error: app.h" << SDL_GetError() << std::endl; 
	} else {
		std::cerr << message << std::endl;
	}
}
