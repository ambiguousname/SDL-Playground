#include "game.h"
#include "util/controls/keyboard.h"

using namespace std;

SDL_Rect player;
Controller player_controller;
shared_ptr<Control> move_control;

void update(App& app) {
	player_controller.update();
	Vector2 move_val = move_control->getValue().vec;
	player.x += move_val.x;
	player.y += move_val.y;
	SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 0xff);
	SDL_RenderClear(app.renderer);
	SDL_SetRenderDrawColor(app.renderer, 0xff, 0, 0, 0xff);
	SDL_RenderFillRect(app.renderer, &player);
	SDL_RenderPresent(app.renderer);
}

ExitCode game(App& app) {
	player = SDL_Rect{10, 10, 10, 10};
	player_controller = Controller {};
	const char* keys[4] = {"W", "S", "A", "D"};
	const char* arrows[4] = {"Up", "Down", "Left", "Right"};
	player_controller.bindControl("move", VECTOR2, 2, KeyVector(keys), KeyVector(arrows));
	move_control = player_controller.getControl("move");
	return app.update(update);
}