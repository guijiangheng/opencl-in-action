add_executable(sub_buffer sub_buffer.cpp)
set_target_properties(sub_buffer PROPERTIES FOLDER "08")
add_custom_command(
    TARGET sub_buffer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/sub_buffer.cl
        ${CMAKE_CURRENT_BINARY_DIR}/sub_buffer.cl
)
