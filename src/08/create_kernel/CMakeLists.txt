add_executable(create_kernel create_kernel.cpp)
set_target_properties(create_kernel PROPERTIES FOLDER "08")
add_custom_command(
    TARGET create_kernel POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/create_kernel.cl
        ${CMAKE_CURRENT_BINARY_DIR}/create_kernel.cl
)
