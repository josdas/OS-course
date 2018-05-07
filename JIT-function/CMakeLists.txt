cmake_minimum_required(VERSION 3.9)
project(HW03)

set(CMAKE_CXX_STANDARD 14)

add_executable(HW03 compiler.cpp jit_function.h memory_handler.h)