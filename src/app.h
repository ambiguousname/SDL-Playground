#pragma once
#include <SDL.h>
#include "util/controls/controller.h"

enum ExitCode {
	SUCCESS,
	SDL_CREATION_FAILED,
};

class App {
	SDL_Window* window;

	public:
	SDL_Renderer* renderer;
	App(ExitCode&);
	~App();
	ExitCode update(void (*)(App&));
};