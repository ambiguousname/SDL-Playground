#pragma once
#include "object.hpp"

class Mesh : public Object {
	protected:
	void _update() override;
	void _destroy() override;
	public:
	glm::mat4 position;
	Mesh(App& app);
	Mesh() {}
};