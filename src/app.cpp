#include "app.h"

void logErr(std::string message) {
	std::cout << message << " Error: " << SDL_GetError() << std::endl; 
}

App::App(ExitCode& success) {
	window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (window == nullptr) {
		logErr("Could not create window.");
		success = SDL_CREATION_FAILED;
		return;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) {
		logErr("Could not create renderer.");
		success = SDL_CREATION_FAILED;
		return;
	}
	success = SUCCESS;
	return;
}

App::~App() {
	SDL_DestroyWindow(window);
	SDL_Quit();
}

ExitCode App::update(void (*update)(App&)) {
	bool quit = false;
	while(!quit) {
		SDL_Event e;
		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT) {
				quit = true;
				break;
			}

			if (e.type == SDL_KEYDOWN) {
			}
		}
		update(*this);
	}
	return SUCCESS;
}