add_executable(id_check id_check.cpp)
set_target_properties(id_check PROPERTIES FOLDER "05")
add_custom_command(
    TARGET id_check POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/id_check.cl
        ${CMAKE_CURRENT_BINARY_DIR}/id_check.cl
)
