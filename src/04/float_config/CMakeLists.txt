add_executable(float_config float_config.cpp)
target_link_libraries(float_config opencl)
set_target_properties(float_config PROPERTIES FOLDER "04")
