#pragma once
#include "app.h"

class Object {
	void* inner;
	void* renderer;
	Context context;
	public:
	Object(App& app);
	Object() {}
	~Object();
};