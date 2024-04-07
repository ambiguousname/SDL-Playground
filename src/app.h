#pragma once
#include <SDL.h>
#include <iostream>
#include <string>

enum ExitCode {
	SUCCESS,
	SDL_CREATION_FAILED,
};

void logErr(std::string message);

struct App {
	SDL_Window* window;
	SDL_Renderer* renderer;

	App(ExitCode&);
	~App();
	ExitCode update(void (*)(App&));
};