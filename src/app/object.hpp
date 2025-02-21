#pragma once
#include "app.hpp"

class Object {
	protected:
	void* inner = nullptr;
	void* renderer = nullptr;
	Context context;

	bool isActive = false;

	virtual void _create() {}
	virtual void _update() {}
	virtual void _destroy() {}
	
	public:
	Object(App& app);
	Object() {}
	void update();
	void destroy();
};