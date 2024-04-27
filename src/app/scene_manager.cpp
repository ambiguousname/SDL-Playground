#include "scene_manager.h"

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
			VulkanShader<VulkanVertex> vert(device->ptr, "shaders/vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
			VulkanShader<VulkanVertex> frag(device->ptr, "shaders/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
			ShaderDescription<VulkanVertex> description({vert, frag});
			renderer->attachGraphicsPipelineInfo(VulkanPipeline::getPipelineInfo(renderer, description.getShaderStages(), description.vertexInfo));
			description.destroy();
			break;
	}
	activeScene->create();
}

void SceneManager::update() {
	if (activeScene != nullptr) {
		activeScene->update();
	}
}