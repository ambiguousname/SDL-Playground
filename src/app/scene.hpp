#pragma once
#include "app.hpp"

class App;

class IScene {
	protected:
	App& app;
	public:
	IScene(App& app) : app(app) {}
	virtual void preload() {}
	virtual void create() {}
	virtual void update() {}
};