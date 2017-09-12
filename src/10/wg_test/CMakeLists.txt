add_executable(wg_test wg_test.cpp)
set_target_properties(wg_test PROPERTIES FOLDER "10")
add_custom_command(
    TARGET wg_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/wg_test.cl
        ${CMAKE_CURRENT_BINARY_DIR}/wg_test.cl
)
