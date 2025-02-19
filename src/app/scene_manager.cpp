#include "scene_manager.hpp"
#include "vulkan/object.hpp"

SceneManager::SceneManager(App* app) : app(app) {

}

SceneManager::~SceneManager() {
	delete activeScene;
}

void SceneManager::initializeScene() {
	activeScene->preload();
	// switch (app->getContext()) {
	// 	case VULKAN:
	// 		break;
	// }
	activeScene->create();
	app->update();
}

void SceneManager::update() {
	switch(app->getContext()) {
		case VULKAN:
			app->vulkanInstance->draw();
		break;
	}
	if (activeScene != nullptr) {
		activeScene->update();
	}
}