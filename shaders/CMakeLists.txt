set(SHADER_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/main.frag ${CMAKE_CURRENT_SOURCE_DIR}/main.vert)

add_custom_target(shaders
	COMMAND glslc -c ${SHADER_SOURCE}
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
# TODO: Copy the files to the Debug/shaders directory.