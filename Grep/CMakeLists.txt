cmake_minimum_required(VERSION 2.4)
project(OS)

set(CMAKE_CXX_STANDARD 14)

add_executable(OS grep.cpp path.h autoclosable.h str_searcher.h utils.h)