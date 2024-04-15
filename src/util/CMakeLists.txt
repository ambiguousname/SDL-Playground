target_sources(SDLPlayground PRIVATE logger.cpp controls/controller.cpp controls/keyboard.cpp PUBLIC logger.h events.h macros.h math.h controls/controller.h controls/keyboard.h)


add_custom_target(gamecontrollerdb
	COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/controls/gamecontrollerdb.txt ${CMAKE_BINARY_DIR}/bin/gamecontrollerdb.txt
)
add_dependencies(SDLPlayground gamecontrollerdb)