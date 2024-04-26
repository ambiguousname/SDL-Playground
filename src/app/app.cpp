#include "app.h"
#include "errors.h"


App::App(const char* name, Context ctx) {
	this->name = name;
	this->context = ctx;

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		throw AppError("Could not init SDL.");
	}
	window = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE );
	if (window == nullptr) {
		throw SDLError("Could not create window.");
	}

	switch (ctx) {
		case VULKAN:
			vulkanInstance = new VulkanWrapper(this);
			break;
	}
	return;
}

App::~App() {
	SDL_DestroyWindow(window);
	switch(context) {
		case HARDWARE_2D:
			SDL_DestroyRenderer(sdlRenderer);
			break;
		case VULKAN:
			delete vulkanInstance;
			break;
	}
	SDL_Quit();
}

void App::update(void (*update)(App&)) {
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
		switch(context) {
			case VULKAN:
				vulkanInstance->draw();
			break;
		}
	}
	// Destroy before destructors, since we want a certain order.
	switch(context) {
		case VULKAN:
			vulkanInstance->destroy();
		break;
	}
}

void* App::createRenderer() {
	switch (context) {
		case VULKAN:
			return (void*)vulkanInstance->createRenderer();
		break;
		case HARDWARE_2D:
			sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (sdlRenderer == nullptr) {
				throw SDLError("Could not create renderer.");
			}
			return (void*) sdlRenderer;
	}
	throw AppError("No renderer found.");
}