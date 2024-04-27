#pragma once
#include <SDL_error.h>

class AppError : public std::exception {
	protected:
	std::string _msg;

	public:
	
	explicit AppError(const char* message) : _msg(message) {}
	explicit AppError(std::string& message) : _msg(message) {}
	virtual ~AppError() noexcept {}

	virtual const char* what() const throw() {
		return std::string("Mekleth Error: " + _msg).c_str();
	}
};

class SDLError : public AppError {
	public:
	
	explicit SDLError(const char* message) : AppError(message) {}
	explicit SDLError(std::string message) : AppError(message) {}

	virtual const char* what() const throw() override {
		return std::string("An SDL Error occured: " +  _msg + " - " + SDL_GetError()).c_str();
	}
};