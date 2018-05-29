//
// Created by josdas on 21.05.18.
//

#ifndef CLIENT_SERVER_UTILS_H
#define CLIENT_SERVER_UTILS_H

#include <stdexcept>
#include <string>


void log_print(std::string const &str);

void log_print(std::exception const &e);

std::runtime_error create_error(std::string const &what);

#endif //CLIENT_SERVER_UTILS_H
