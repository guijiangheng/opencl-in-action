add_executable(profile_read profile_read.cpp)
set_target_properties(profile_read PROPERTIES FOLDER "07")
add_custom_command(
    TARGET profile_read POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/profile_read.cl
        ${CMAKE_CURRENT_BINARY_DIR}/
)
