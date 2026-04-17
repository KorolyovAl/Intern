cmake_minimum_required(VERSION 3.22)

project(log_filter)

# Set the C++ standard to 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(log_filter_core
            src/log_filter.cpp
            src/log_filter.h
            src/filter_impl.cpp
            src/filter_impl.h
)

target_include_directories(log_filter_core PUBLIC
                            ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(log_filter 
                src/main.cpp
                )

target_link_libraries(log_filter PRIVATE log_filter_core)

option(LOG_FILTER_BUILD_TESTS "Build tests" OFF)

if (LOG_FILTER_BUILD_TESTS)
    include(CTest)
    add_subdirectory(tests)
endif()