target_sources(SDLPlayground PRIVATE app.cpp object.cpp scene_manager.cpp PUBLIC app.hpp errors.hpp object.hpp scene.hpp scene_manager.hpp)

add_subdirectory(vulkan)