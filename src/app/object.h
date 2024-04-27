#pragma once
#include "app.h"

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