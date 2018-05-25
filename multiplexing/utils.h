//
// Created by josdas on 21.05.18.
//

#ifndef CLIENT_SERVER_UTILS_H
#define CLIENT_SERVER_UTILS_H

#include <stdexcept>
#include <string>
#include <vector>

#include "scoped_socket.h"
#include "buffered_socket.h"


const char * const MESSAGE_END = "\r\n";
const size_t BUFFER_SIZE_ = 2048;

void log_print(std::string const &str);

void log_print(std::exception const &e);

std::runtime_error create_error(std::string const &what);

uint32_t string_to_address(std::string const &str);

std::vector<std::string> tokenize(std::string const &str, char sep);

void send(ScopedSocket &socket, std::string const &message);

void send(BufferedSocket &socket, std::string const &message);

#endif //CLIENT_SERVER_UTILS_H
