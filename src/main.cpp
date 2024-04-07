#include "game.h"

int main(int argc, char* args[]) {
	ExitCode status;
	App app = App(status);
	if (status != SUCCESS) {
		return status;
	}
	status = game(app);
	return status;
}