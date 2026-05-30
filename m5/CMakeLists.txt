cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(thread_pool)

add_executable(thread_pool 
                src/main.cpp
                src/thread_pool.h
              )