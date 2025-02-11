cmake_minimum_required(VERSION 3.30.4)
project(SDLPlayground)

find_package(SDL2 CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(SDLPlayground WIN32 ${SOURCE})
add_subdirectory(src)

set_property(TARGET SDLPlayground PROPERTY CXX_STANDARD 20)

if(TARGET SDL2::SDL2main)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    target_link_libraries(SDLPlayground PRIVATE SDL2::SDL2main)
endif()

target_link_libraries(SDLPlayground PRIVATE Vulkan::Vulkan SDL2::SDL2 glm::glm)