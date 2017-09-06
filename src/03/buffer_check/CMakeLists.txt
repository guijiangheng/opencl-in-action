add_executable(buffer_check buffer_check.cpp)
target_link_libraries(buffer_check opencl)
set_target_properties(buffer_check PROPERTIES FOLDER "03")
