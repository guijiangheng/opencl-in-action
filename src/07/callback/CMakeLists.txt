add_executable(callback callback.cpp)
set_target_properties(callback PROPERTIES FOLDER "07")
add_custom_command(
    TARGET callback POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/callback.cl
        ${CMAKE_CURRENT_BINARY_DIR}/
)
