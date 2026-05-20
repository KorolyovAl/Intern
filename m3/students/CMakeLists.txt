cmake_minimum_required(VERSION 3.12)

project(students)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(students_lib
            src/students.cpp
            src/students.h    
)

target_include_directories(students_lib PUBLIC
                            ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(students src/main.cpp)

target_link_libraries(students PRIVATE students_lib)

option(BUILD_TESTING OFF)

if (BUILD_TESTING)
    include(CTest)
    add_subdirectory(tests)
endif()