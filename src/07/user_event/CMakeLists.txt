add_executable(user_event user_event.cpp)
target_link_libraries(user_event opencl)

set_target_properties(user_event PROPERTIES FOLDER "07")
add_custom_command(
    TARGET user_event POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/user_event.cl
        ${CMAKE_CURRENT_BINARY_DIR}/
)
