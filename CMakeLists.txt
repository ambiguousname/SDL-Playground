cmake_minimum_required(VERSION 3.25.1)
project(SDLPlayground)

find_package(SDL2 REQUIRED)
include_directories(SDLPlayground ${SDL2_INCLUDE_DIRS})

set(SOURCE src/main.cpp src/game.cpp src/app.cpp)

add_executable(SDLPlayground ${SOURCE})
target_link_libraries(SDLPlayground ${SDL2_LIBRARIES})