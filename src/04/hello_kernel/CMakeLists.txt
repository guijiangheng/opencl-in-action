add_executable(hello_kernel hello_kernel.cpp)
target_link_libraries(hello_kernel opencl)

set_target_properties(hello_kernel PROPERTIES FOLDER "04")
add_custom_command(
    TARGET hello_kernel POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/hello_kernel.cl
        ${CMAKE_CURRENT_BINARY_DIR}/hello_kernel.cl
)
