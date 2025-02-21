#pragma once
#include "object.hpp"

class Mesh : public Object {
	protected:
	void _destroy() override;
	glm::mat4* position;
	public:
	void setPosition(glm::mat4 p) { *position = p; }
	glm::mat4 getPosition() { return *position; }
	void translate(glm::vec3 translation) { *position = glm::translate(*position, translation); }
	Mesh(App& app);
	Mesh() {}
};