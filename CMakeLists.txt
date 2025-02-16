cmake_minimum_required(VERSION 3.30.4)
project(SDLPlayground)

set(APP_DIR ${CMAKE_BINARY_DIR}/Debug)

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)
find_package(Vulkan REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(SDLPlayground WIN32 ${SOURCE})
add_subdirectory(src)

add_subdirectory(shaders)
add_dependencies(SDLPlayground shaders)

set_property(TARGET SDLPlayground PROPERTY CXX_STANDARD 20)

target_link_libraries(SDLPlayground PRIVATE Vulkan::Vulkan SDL3::SDL3 glm::glm)