add_executable(profile profile.cpp)
set_target_properties(profile PROPERTIES FOLDER "08")
add_custom_command(
    TARGET profile POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/profile.cl
        ${CMAKE_CURRENT_BINARY_DIR}/profile.cl
)
