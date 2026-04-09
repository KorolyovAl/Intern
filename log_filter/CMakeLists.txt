cmake_minimum_required(VERSION 3.22)

project(log_filter)

# Set the C++ standard to 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()

add_executable(log_filter 
                src/main.cpp
                src/log_filter.cpp
                src/log_filter.h
                )