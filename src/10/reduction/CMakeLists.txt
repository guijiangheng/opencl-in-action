add_executable(reduction reduction.cpp)
set_target_properties(reduction PROPERTIES FOLDER "10")
add_custom_command(
    TARGET reduction POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/reduction.cl
        ${CMAKE_CURRENT_BINARY_DIR}/reduction.cl
)
