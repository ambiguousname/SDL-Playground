#pragma once
#include "scene.hpp"
#include "app.hpp"

class App;

template<class T>
concept Scene = std::is_base_of<IScene, T>::value;

class SceneManager {
	IScene* activeScene = nullptr;
	App* app;

	void initializeScene();

	public:
	SceneManager(App* app);
	~SceneManager();
	template<Scene T>
	void loadScene() {
		if (activeScene != nullptr) {
			delete activeScene;
		}
		activeScene = new T(*app);
		initializeScene();
	}

	void update();
};