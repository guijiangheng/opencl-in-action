add_executable(simple_image simple_image.cpp)
set_target_properties(simple_image PROPERTIES FOLDER "06")
add_custom_command(
    TARGET simple_image POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/simple_image.cl ${CMAKE_CURRENT_SOURCE_DIR}/blank.png
        ${CMAKE_CURRENT_BINARY_DIR}/
)
