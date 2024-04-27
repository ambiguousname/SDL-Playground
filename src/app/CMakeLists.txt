target_sources(SDLPlayground PRIVATE app.cpp object.cpp scene_manager.cpp PUBLIC app.h errors.h object.h scene.h scene_manager.h)

add_subdirectory(vulkan)