cmake_minimum_required(VERSION 3.11)

project(my_ptr)
set(CMAKE_CXX_STANDART 20)

add_executable(my_ptr 
    src/main.cpp
    src/my_shared_ptr.h
    src/my_weak_ptr.h
    src/control_block.h
)