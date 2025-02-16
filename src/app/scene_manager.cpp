#include "scene_manager.hpp"

SceneManager::SceneManager(App* app) : app(app) {

}

SceneManager::~SceneManager() {
	delete activeScene;
}

void SceneManager::initializeScene() {
	activeScene->preload();
	switch (app->getContext()) {
		case VULKAN:
			VulkanRenderer* renderer = (VulkanRenderer*)app->getRenderer();
			const VulkanLogicDevice* device = renderer->getDevice();

			// Default pipeline:
			VulkanShader vert(device->ptr, "shaders/main.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
			VulkanShader frag(device->ptr, "shaders/main.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
			ShaderDescription description = ShaderDescription(std::vector<VulkanShader>({vert, frag}));
			description.attachDescription<VulkanVertex>();
			renderer->attachPendingGraphicsPipeline(new VulkanPipelineInfo(renderer, description));

			renderer->intializePipelines();
			break;
	}
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