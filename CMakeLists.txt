cmake_minimum_required(VERSION 3.24)
set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE STRING "Minimum macOS version")
project(orbital-bass-engine VERSION 1.5.0)

if(NOT CMAKE_CXX_COMPILER_LAUNCHER)
    find_program(CCACHE_PROGRAM ccache)
    if(CCACHE_PROGRAM)
        set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    endif()
endif()

add_subdirectory(modules/JUCE)
add_subdirectory(src)
