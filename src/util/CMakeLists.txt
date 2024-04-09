FetchContent_Declare(
	gamecontrollerdb
	URL https://raw.githubusercontent.com/mdqinc/SDL_GameControllerDB/master/gamecontrollerdb.txt
)

target_sources(SDLPlayground PRIVATE logger.cpp controls/controller.cpp PUBLIC logger.h events.h macros.h math.h controls/controller.h)
