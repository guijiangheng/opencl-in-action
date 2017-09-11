cmake_minimum_required(VERSION 3.2)
project(opencl-in-action CXX)

set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})

include_directories(ext/libpng/include)
link_directories(ext/libpng/lib)
link_libraries(libpng.lib zlib.lib ${OpenCL_LIBRARIES})

macro(subdirs ret)
    file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*/*)
    foreach(child ${children})
        if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child})
            list(APPEND ${ret} ${child})
        endif()
    endforeach()
endmacro()

subdirs(dirs)
foreach(dir ${dirs})
    add_subdirectory(${dir})
endforeach()
