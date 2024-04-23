#pragma once
#include <SDL.h>
#include "../util/controls/controller.h"
#include "vulkan/vulkan.h"

enum Context {
	VULKAN,
	HARDWARE_2D
};

class VulkanWrapper;
class VulkanRenderer;

class App {
	SDL_Window* window;
	Context context;

	friend class VulkanWrapper;
	friend class OpenGLWrapper;

	public:
	const char* name;

	union {
		SDL_Renderer* sdlRenderer;
		VulkanWrapper* vulkanInstance;
	};
	
	App(const char* name, Context ctx);
	~App();
	void update(void (*)(App&));
};