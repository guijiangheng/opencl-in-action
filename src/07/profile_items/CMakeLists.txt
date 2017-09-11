add_executable(profile_items profile_items.cpp)
set_target_properties(profile_items PROPERTIES FOLDER "07")
add_custom_command(
    TARGET profile_items POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/profile_items.cl
        ${CMAKE_CURRENT_BINARY_DIR}/
)
