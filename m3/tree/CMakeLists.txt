cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(RBTree)

add_executable(RBTreee
                src/main.cpp
                src/myTree.h
)
