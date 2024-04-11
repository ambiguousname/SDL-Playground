cmake_minimum_required(VERSION 3.25.1)
project(SDLPlayground)

find_package(SDL2 REQUIRED)
include_directories(SDLPlayground ${SDL2_INCLUDE_DIRS})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(SDLPlayground ${SOURCE})
add_subdirectory(src)

target_link_libraries(SDLPlayground
 ${SDL2_LIBRARIES})