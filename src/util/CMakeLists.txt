target_sources(SDLPlayground PRIVATE controls/controller.cpp controls/keyboard.cpp PUBLIC events.hpp macros.hpp controls/controller.hpp controls/keyboard.hpp)


configure_file(${CMAKE_CURRENT_SOURCE_DIR}/controls/gamecontrollerdb.txt ${CMAKE_BINARY_DIR}/gamecontrollerdb.txt COPYONLY)