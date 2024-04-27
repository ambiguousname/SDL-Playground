#include "game.h"
#include <iostream>

int main(int argc, char* args[]) {
	App app = App(AppConfig{"Game", VULKAN});
	app.sceneManager.loadScene<Main>();
	return 0;
}