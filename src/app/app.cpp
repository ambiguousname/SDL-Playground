#include "app.h"
#include "errors.h"

App::App(AppConfig config) : sceneManager(SceneManager(this)) {
	this->name = config.name;
	this->context = config.context;

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		throw AppError("Could not init SDL.");
	}

	switch (context) {
		case VULKAN:
			vulkanInstance = new VulkanWrapper(this);
			break;
	}

	window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE );
	if (window == nullptr) {
		throw SDLError("Could not create window.");
	}

	switch (context) {
		case VULKAN:
			vulkanInstance->createRenderer();
			break;
		case HARDWARE_2D:
			sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (sdlRenderer == nullptr) {
				throw SDLError("Could not create renderer.");
			}
			break;
	}
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

void App::update() {
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
		sceneManager.update();
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

void* App::getRenderer() {
	switch (context) {
		case VULKAN:
			return (void*)vulkanInstance->getRenderer();
			break;
		case HARDWARE_2D:
			return (void*)sdlRenderer;
			break;
	}
}