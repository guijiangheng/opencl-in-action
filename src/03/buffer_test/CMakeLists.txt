add_executable(buffer_test buffer_test.cpp)
target_link_libraries(buffer_test opencl)
set_target_properties(buffer_test PROPERTIES FOLDER "03")
