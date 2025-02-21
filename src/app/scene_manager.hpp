#pragma once
#include "scene.hpp"
#include <type_traits>

class App;

template<class T>
concept Scene = std::is_base_of<IScene, T>::value;

class SceneManager {
	IScene* activeScene = nullptr;
	App* app;

	bool isActive = false;

	void initializeScene();

	public:
	SceneManager(App* app);
	template<Scene T>
	void loadScene() {
		if (activeScene != nullptr) {
			delete activeScene;
		}
		activeScene = new T(*app);
		initializeScene();
	}

	void destroy() {
		activeScene->destroy();
		delete activeScene;
		activeScene = nullptr;
	}

	void update();
};