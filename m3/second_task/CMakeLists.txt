cmake_minimum_required(VERSION 3.12)

project(numArray)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(numArray
                src/main.cpp
                src/number_array.h
)