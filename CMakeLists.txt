cmake_minimum_required(VERSION 3.23)
project(ECE650)

set(CMAKE_CXX_STANDARD 14)
MATH(EXPR stack_size "4*1024*1024")

add_executable(ECE650 main.cpp)
