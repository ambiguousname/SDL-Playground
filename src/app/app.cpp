#include "app.hpp"
#include "errors.hpp"

App::App(AppConfig config) : sceneManager(*new SceneManager(this)) {
	this->name = config.name;
	this->context = config.context;

	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD )) {
		throw AppError("Could not init SDL.");
	}

	window = SDL_CreateWindow(name.c_str(), 640, 480, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE );
	if (window == nullptr) {
		throw SDLError("Could not create window.");
	}

	switch (context) {
		case VULKAN:
			vulkanInstance = new VulkanWrapper(this);
			vulkanInstance->createRenderer();
			break;
		case HARDWARE_2D:
			sdlRenderer = SDL_CreateRenderer(window, nullptr);
			if (sdlRenderer == nullptr) {
				throw SDLError("Could not create renderer.");
			}
			break;
	}
}

App::~App() {
	delete &sceneManager;
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
			if (e.type == SDL_EVENT_QUIT) {
				quit = true;
				break;
			}
		}
		SDL_PumpEvents();
		sceneManager.update();
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
	return nullptr;
}