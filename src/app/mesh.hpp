#pragma once
#include "object.hpp"

class Mesh : public Object {
	protected:
	void _destroy() override;
	glm::mat4* transform;
	public:
	inline void setPosition(glm::vec3 p) { 
		(*transform)[3] = glm::vec4(p.x, p.y, p.z, 1.0f);
	}
	inline glm::vec3 getPosition() { return (*transform)[3]; }
	inline void translate(glm::vec3 translation) { *transform = glm::translate(*transform, translation); }
	Mesh(App& app);
	Mesh() {}
};