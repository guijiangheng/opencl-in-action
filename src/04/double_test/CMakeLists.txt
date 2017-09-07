add_executable(double_test double_test.cpp)
target_link_libraries(double_test opencl)

set_target_properties(double_test PROPERTIES FOLDER "04")
add_custom_command(
    TARGET double_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/double_test.cl
        ${CMAKE_CURRENT_BINARY_DIR}/double_test.cl
)
