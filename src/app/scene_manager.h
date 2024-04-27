#pragma once
#include "scene.h"
#include "app.h"

class App;

class SceneManager {
	IScene* activeScene = nullptr;
	App* app;

	void initializeScene();

	public:
	SceneManager(App* app);
	~SceneManager();
	template<class T>
	void loadScene() {
		if (activeScene != nullptr) {
			delete activeScene;
		}
		activeScene = new T(*app);
	}

	void update();
};