cmake_minimum_required(VERSION 3.11)
project(myPointers LANGUAGES CXX)

# Set the C++ standard to 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(my_ptr 
    src/main.cpp
    src/my_shared_ptr.h
    src/my_weak_ptr.h
    src/control_block.h
)

target_include_directories(my_ptr PRIVATE src)