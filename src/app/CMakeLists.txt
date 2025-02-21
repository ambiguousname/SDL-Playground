target_sources(SDLPlayground PRIVATE app.cpp object.cpp mesh.cpp camera.cpp scene_manager.cpp PUBLIC app.hpp errors.hpp object.hpp mesh.hpp camera.hpp scene.hpp scene_manager.hpp)

add_subdirectory(vulkan)