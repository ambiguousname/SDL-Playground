cmake_minimum_required(VERSION 3.31.0)
project(SDLPlayground)

find_package(SDL2 CONFIG REQUIRED)
find_package(VulkanHeaders CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(SDLPlayground ${SOURCE})
add_subdirectory(src)

set_property(TARGET SDLPlayground PROPERTY CXX_STANDARD 20)

target_link_libraries(SDLPlayground
 SDL2::SDL2 Vulkan::Headers glm::glm)