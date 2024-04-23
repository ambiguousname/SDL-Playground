cmake_minimum_required(VERSION 3.25.1)
project(SDLPlayground)

find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)
include_directories(SDLPlayground ${Vulkan_INCLUDE_DIR} ${SDL2_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_BUILD_TYPE Debug)

add_executable(SDLPlayground ${SOURCE})
add_subdirectory(src)

set_property(TARGET SDLPlayground PROPERTY CXX_STANDARD 20)

target_link_libraries(SDLPlayground
 ${SDL2_LIBRARIES} ${Vulkan_LIBRARIES} glm)