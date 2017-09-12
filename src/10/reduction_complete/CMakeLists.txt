add_executable(reduction_complete reduction_complete.cpp)
set_target_properties(reduction_complete PROPERTIES FOLDER "10")
add_custom_command(
    TARGET reduction_complete POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/reduction_complete.cl
        ${CMAKE_CURRENT_BINARY_DIR}/reduction_complete.cl
)
