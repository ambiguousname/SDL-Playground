#include "app.h"
#include "util/logger.h"

App::App(ExitCode& success) {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		logErr(SDL, "Could not init SDL.");
		success = SDL_CREATION_FAILED;
		return;
	}
	window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (window == nullptr) {
		logErr(SDL, "Could not create window.");
		success = SDL_CREATION_FAILED;
		return;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) {
		logErr(SDL, "Could not create renderer.");
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
		}
		SDL_PumpEvents();
		update(*this);
	}
	return SUCCESS;
}