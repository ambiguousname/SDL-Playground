#pragma once
#include "object.hpp"

class Mesh : public Object {
	protected:
	void _update() override {}
	void _destroy() override;
	public:
	Mesh(App& app);
	Mesh() {}
};