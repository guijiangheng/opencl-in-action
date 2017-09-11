add_executable(buffer_test_2 buffer_test.cpp)
set_target_properties(buffer_test_2 PROPERTIES FOLDER "08")
add_custom_command(
    TARGET buffer_test_2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/buffer_test.cl
        ${CMAKE_CURRENT_BINARY_DIR}/buffer_test.cl
)
