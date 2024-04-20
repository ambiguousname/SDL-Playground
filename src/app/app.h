#pragma once
#include <SDL.h>
#include "../util/controls/controller.h"
#include "vulkan.h"

class AppError : public std::exception {
	protected:
	std::string _msg;

	public:
	
	explicit AppError(const char* message) : _msg(message) {}
	explicit AppError(std::string message) : _msg(message) {}

	virtual const char* what() const throw() {
		return ("An Application Error occured: " + _msg).c_str();
	}
};

class SDLError : public AppError {
	public:
	
	explicit SDLError(const char* message) : AppError(message) {}
	explicit SDLError(std::string message) : AppError(message) {}

	virtual const char* what() const throw() {
		return ("An SDL Error occured: " +  _msg + " - " + SDL_GetError()).c_str();
	}
};

enum Context {
	VULKAN,
	HARDWARE_2D,
	OPENGL
};

class VulkanWrapper;

class App {
	SDL_Window* window;
	Context context;

	friend class VulkanWrapper;

	public:
	const char* name;

	union {
		SDL_Renderer* renderer;
		VulkanWrapper* instance;
	};
	
	App(const char* name, Context ctx);
	~App();
	void update(void (*)(App&));
};