cmake_minimum_required(VERSION 3.7)
project(opencl-in-action CXX)

set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

include_directories(ext/opencl/include)
link_directories(ext/opencl/lib/x86_64)

add_subdirectory(src)
