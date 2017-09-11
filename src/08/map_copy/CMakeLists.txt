add_executable(map_copy_2 map_copy.cpp)
set_target_properties(map_copy_2 PROPERTIES FOLDER "08")
add_custom_command(
    TARGET map_copy_2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/map_copy.cl
        ${CMAKE_CURRENT_BINARY_DIR}/map_copy.cl
)
