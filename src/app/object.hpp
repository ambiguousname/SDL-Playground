#pragma once
#include "app.hpp"

struct ObjectConfig {
	
};

class Object {
	void* inner;
	void* renderer;
	Context context;
	public:
	Object(App& app);
	Object() {}
	void destroy();
};