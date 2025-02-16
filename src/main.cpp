#include "game.hpp"
#include <iostream>
#include <SDL3/SDL_main.h>

int main(int argc, char* args[]) {
	App app = App(AppConfig{"Game", VULKAN});
	app.sceneManager.loadScene<Main>();
	return 0;
}