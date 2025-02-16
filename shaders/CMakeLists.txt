set(SHADER_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/main.frag ${CMAKE_CURRENT_SOURCE_DIR}/main.vert)

make_directory(${APP_DIR}/shaders)

add_custom_target(shaders
	COMMAND glslc -c ${SHADER_SOURCE}
	WORKING_DIRECTORY ${APP_DIR}/shaders
)