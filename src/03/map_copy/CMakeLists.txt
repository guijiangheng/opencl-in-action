add_executable(map_copy map_copy.cpp)
target_link_libraries(map_copy opencl)
set_target_properties(map_copy PROPERTIES FOLDER "03")
