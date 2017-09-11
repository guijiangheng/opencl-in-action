add_executable(interp interp.cpp)
set_target_properties(interp PROPERTIES FOLDER "06")
add_custom_command(
    TARGET interp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/interp.cl ${CMAKE_CURRENT_SOURCE_DIR}/input.png
        ${CMAKE_CURRENT_BINARY_DIR}/
)
