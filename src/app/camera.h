#pragma once
#include <glm/glm.hpp>
#include <matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

class Camera {
	protected:
	glm::mat4 projection;
	glm::mat4 view;

	glm::vec3 position = glm::vec3(0.0f, 0.0f, -3.0f);
	glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	float fov;
	float nearZ;
	float farZ;
	float w;
	float h;
	
	void updateProjection() {
		projection = glm::perspective(glm::radians(fov), w/h, nearZ, farZ);
	}
	void update() {
		view = glm::lookAt(position, position + forward, up);
	}

	public:
	Camera(float _w, float _h, float _fov=45.0f, float _nearZ=0.1f, float _farZ=100.0f) {
		fov = _fov;
		nearZ = _nearZ;
		farZ = _farZ;
		w = _w;
		h = _h;

		updateProjection();
		update();
	}
	Camera() {}

	void getCameraMatrices(const glm::mat4*& out_projection, const glm::mat4*& out_view) const {
		out_projection = &projection;
		out_view = &view;
	}

	void translate(glm::vec3 add) {
		position += add;
		update();
	}
};