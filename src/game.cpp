#include "game.h"
#include "util/controls/keyboard.h"

SDL_Rect player;
Controller player_controller;

void update(App& app) {
	player_controller.update();
	SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 0xff);
	SDL_RenderClear(app.renderer);
	SDL_SetRenderDrawColor(app.renderer, 0xff, 0, 0, 0xff);
	SDL_RenderFillRect(app.renderer, &player);
	SDL_RenderPresent(app.renderer);
}

void upPress(ControlDataOut out) {
	if (out.data.button == true) {
		player.y += 1;
	}
}

ExitCode game(App& app) {
	player = SDL_Rect{10, 10, 10, 10};
	player_controller = Controller {};
	std::unique_ptr<Control> up = std::make_unique<Control>(BOOL);
	up->addSource(std::make_unique<KeySource>("W"));
	up->addSource(std::make_unique<KeySource>("Up"));
	player_controller.bindControl("up", std::move(up));
	player_controller.listenForControl("up", Event<ControlDataOut>{upPress});
	return app.update(update);
}