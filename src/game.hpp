#pragma once
#include "app/app.hpp"
#include "app/mesh.hpp"
#include "app/camera.hpp"
#include "util/controls/controller.hpp"
#include <glm/glm.hpp>
#include <memory>

class Main : public IScene {
	SDL_Rect player;
	Controller player_controller;
	std::shared_ptr<Control> move_control;
	Mesh test;
	Camera cam;

	public:
	Main(App& app) : IScene(app) {}
	void preload() override;
	void create() override;
	void update() override;
	// TODO: Make the scene handle this automatically.
	void destroy() override;
};