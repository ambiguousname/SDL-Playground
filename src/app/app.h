#pragma once
#include <SDL.h>
#include "../util/controls/controller.h"
#include "vulkan/vulkan.h"

class AppError : public std::exception {
	std::string _msg;

	public:
	
	explicit AppError(const char* message) : _msg(message) {}
	explicit AppError(std::string& message) : _msg(message) {}
	virtual ~AppError() noexcept {}

	virtual const char* what() const throw() {
		return _msg.c_str();
	}
};

class SDLError : public AppError {
	std::string _msg;
	public:
	
	explicit SDLError(const char* message) : AppError(message) {}
	explicit SDLError(std::string message) : AppError(message) {}

	virtual const char* what() const throw() override {
		return ("An SDL Error occured: " +  _msg + " - " + SDL_GetError()).c_str();
	}
};

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