#pragma once
#include "app/app.h"
#include <glm/glm.hpp>

class Main : public IScene {
	SDL_Rect player;
	Controller player_controller;
	shared_ptr<Control> move_control;
	Object test;

	public:
	void preload() override;
	void update() override;
};