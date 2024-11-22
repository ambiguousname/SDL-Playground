#pragma once
#include <SDL.h>
#include "../util/controls/controller.hpp"
#include "vulkan/vulkan.hpp"
#include "scene_manager.hpp"

enum Context {
	VULKAN,
	HARDWARE_2D
};

class VulkanWrapper;
class VulkanRenderer;
class SceneManager;

struct AppConfig {
	std::string name = "Game";
	Context context;
};

class App {
	SDL_Window* window;
	Context context;

	friend class VulkanWrapper;

	public:
	std::string name;
	SceneManager& sceneManager;

	union {
		SDL_Renderer* sdlRenderer;
		VulkanWrapper* vulkanInstance;
	};
	
	const Context getContext() const { return context; }
	void* getRenderer();
	
	App(AppConfig config);
	~App();
	void update();
};