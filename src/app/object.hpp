#pragma once
#include "app.hpp"

class Object {
	void* inner;
	void* renderer;
	Context context;
	public:
	Object(App& app);
	Object() {}
	void destroy();
};