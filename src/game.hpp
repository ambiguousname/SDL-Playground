#pragma once
#include "app/app.hpp"
#include "app/object.hpp"
#include "util/controls/controller.hpp"
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
	void create() override;
	void update() override;
};