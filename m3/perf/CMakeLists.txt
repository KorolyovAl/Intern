cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(perf)

add_executable(perf
                main.cpp
)