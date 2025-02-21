#pragma once
#include <SDL3/SDL.h>
#include "../util/controls/controller.hpp"
#include "vulkan/vulkan.hpp"
#include "scene_manager.hpp"
#include "scene.hpp"

enum Context {
	VULKAN,
	HARDWARE_2D
};

class VulkanWrapper;
class VulkanRenderer;

struct AppConfig {
	std::string name = "Game";
	Context context;
};

class App {
	SDL_Window* window;
	Context context;

	friend class VulkanWrapper;
	
	SceneManager* sceneManager;

	public:
	std::string name;

	union {
		SDL_Renderer* sdlRenderer;
		VulkanWrapper* vulkanInstance;
	};

	template<Scene T>
	void loadScene() {
		sceneManager->loadScene<T>();
	}
	
	const Context getContext() const { return context; }
	void* getRenderer();
	
	App(AppConfig config);
	~App();
	void update();
};