cmake_minimum_required(VERSION 3.9)
project(client_server)

set(CMAKE_CXX_STANDARD 14)

add_executable(client_server main.cpp scoped_socket.h scoped_socket.cpp utils.h utils.cpp server.h client.h server.cpp client.cpp receive_tokenizer.h receive_tokenizer.cpp)