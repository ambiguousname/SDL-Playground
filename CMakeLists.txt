cmake_minimum_required(VERSION 3.25.1)
project(SDLPlayground)
include(FetchContent)

find_package(SDL2 REQUIRED)
include_directories(SDLPlayground ${SDL2_INCLUDE_DIRS})

add_executable(SDLPlayground ${SOURCE})
add_subdirectory(src)

target_link_libraries(SDLPlayground ${SDL2_LIBRARIES})