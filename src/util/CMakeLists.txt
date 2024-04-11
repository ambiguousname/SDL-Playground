target_sources(SDLPlayground PRIVATE logger.cpp controls/controller.cpp PUBLIC logger.h events.h macros.h math.h controls/controller.h)


add_custom_target(gamecontrollerdb
	COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/controls/gamecontrollerdb.txt ${CMAKE_BINARY_DIR}/bin/gamecontrollerdb.txt
)
add_dependencies(SDLPlayground gamecontrollerdb)