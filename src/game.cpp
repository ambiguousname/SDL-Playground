#include "game.h"

SDL_Rect player;

void update(App& app) {
	SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 0xff);
	SDL_RenderClear(app.renderer);
	SDL_SetRenderDrawColor(app.renderer, 0xff, 0, 0, 0xff);
	SDL_RenderFillRect(app.renderer, &player);
	SDL_RenderPresent(app.renderer);
}

ExitCode game(App& app) {
	player = SDL_Rect{10, 10, 10, 10};
	return app.update(update);
}