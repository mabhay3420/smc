# link the generated compile_commands.json file to
# the source directory
add_custom_target(
    symlink_compile_commands ALL
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
    ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
    DEPENDS smc
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)