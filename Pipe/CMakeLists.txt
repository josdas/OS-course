cmake_minimum_required(VERSION 3.9)
project(Pipe)

set(CMAKE_CXX_STANDARD 14)

add_executable(Pipe main.cpp unix_socket.h utils.h utils.cpp unix_socket.cpp pipe.h pipe.cpp server.h server.cpp client.h client.cpp)