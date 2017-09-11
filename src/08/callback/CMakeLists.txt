add_executable(callback_2 callback.cpp)
set_target_properties(callback_2 PROPERTIES FOLDER "08")
add_custom_command(
    TARGET callback_2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/callback.cl
        ${CMAKE_CURRENT_BINARY_DIR}/callback.cl
)
