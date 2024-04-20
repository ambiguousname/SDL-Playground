#include "game.h"
#include <iostream>

int main(int argc, char* args[]) {
	App app = App("Game", VULKAN);
	game(app);
	return 0;
}