#include "game.h"
#include <iostream>

int main(int argc, char* args[]) {
	App app = App("Game", OPENGL);
	game(app);
	return 0;
}