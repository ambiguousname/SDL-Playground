#include "scene_manager.hpp"
#include "vulkan/object.hpp"
#include "app.hpp"

SceneManager::SceneManager(App* app) : app(app) {
}

void SceneManager::initializeScene() {
	isActive = true;

	activeScene->preload();
	// switch (app->getContext()) {
	// 	case VULKAN:
	// 		break;
	// }
	activeScene->create();

	// Start evaluation loop:
	app->update();
}

void SceneManager::update(float delta) {
	if (!isActive) {
		return;
	}
	switch(app->getContext()) {
		case VULKAN:
			app->vulkanInstance->draw();
		break;
	}
	if (activeScene != nullptr) {
		activeScene->update(delta);
	}
}