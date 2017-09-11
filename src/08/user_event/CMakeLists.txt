add_executable(user_event_2 user_event.cpp)
set_target_properties(user_event_2 PROPERTIES FOLDER "08")
add_custom_command(
    TARGET user_event_2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/user_event.cl
        ${CMAKE_CURRENT_BINARY_DIR}/user_event.cl
)
