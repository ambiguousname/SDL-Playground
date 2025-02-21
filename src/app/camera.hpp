#pragma once
#include "app.hpp"
#include "object.hpp"

class Camera : public Object {
	protected:
	/// @brief The nice thing currently about this Vulkan camera setup is that we don't have to keep track of it during the rendering loop, we can update it whenever we want.
	/// This may change with the addition of multiple cameras.
	void _destroy() override;
	public:
	Camera(App& app);
	Camera() {}
};