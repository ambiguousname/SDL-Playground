#include "game.h"
#include "util/controls/keyboard.h"

using namespace std;

SDL_Rect player;
Controller player_controller;
weak_ptr<Control> up;

void update(App& app) {
	player_controller.update();
	if (up.lock()->getValue().data.boolean == true) {
		player.y += 1;
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
	// TODO: Move this into controller functions.
	shared_ptr<Control> upS = make_shared<Control>(BOOL);
	upS->addSource(make_unique<KeySource>("W"));
	upS->addSource(make_unique<KeySource>("Up"));
	player_controller.bindControl("up", move(upS));
	// player_controller.listenForControl("up", Event<ControlDataOut>{upPress});

	up = player_controller.getControl("up");
	return app.update(update);
}