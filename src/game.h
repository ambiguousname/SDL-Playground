#pragma once
#include "app/app.h"
#include "app/object.h"
#include "util/controls/controller.h"
#include <glm/glm.hpp>
#include <memory>

class Main : public IScene {
	SDL_Rect player;
	Controller player_controller;
	std::shared_ptr<Control> move_control;
	Object test;

	public:
	Main(App& app) : IScene(app) {}
	void preload() override;
	void update() override;
};