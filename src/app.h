#pragma once
#include <SDL.h>

enum ExitCode {
	SUCCESS,
	SDL_CREATION_FAILED,
};

class App {
	SDL_Window* window;
	SDL_Renderer* renderer;

	public:
	App(ExitCode&);
	~App();
	ExitCode update(void (*)(App&));
};