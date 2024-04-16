#include "game.h"
#include "util/controls/keyboard.h"

using namespace std;

SDL_Rect player;
Controller player_controller;
shared_ptr<Control> up;
shared_ptr<Control> down;

void update(App& app) {
	player_controller.update();
	if (down->getValue().data.boolean == true) {
		player.y += 1;
	}
	if (up->getValue().data.boolean == true) {
		player.y -= 1;
	}
	SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 0xff);
	SDL_RenderClear(app.renderer);
	SDL_SetRenderDrawColor(app.renderer, 0xff, 0, 0, 0xff);
	SDL_RenderFillRect(app.renderer, &player);
	SDL_RenderPresent(app.renderer);
}

ExitCode game(App& app) {
	player = SDL_Rect{10, 10, 10, 10};
	player_controller = Controller {};
	player_controller.bindControl("up", BOOL, 2, KeySource("W"), KeySource("Up"));
	player_controller.bindControl("down", BOOL, 2, KeySource("S"), KeySource("Down"));
	// player_controller.listenForControl("up", Event<ControlDataOut>{upPress});
	up = player_controller.getControl("up");
	down = player_controller.getControl("down");
	return app.update(update);
}