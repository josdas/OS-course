//
// Created by josdas on 21.05.18.
//

#include <netinet/in.h>

#include <exception>
#include <string>
#include <iostream>
#include <cstring>

#include "utils.h"


using std::string;
using std::cerr;
using std::endl;


void log_print(const std::string &str) {
    cerr << str << endl;
}

void log_print(const std::exception &e) {
    string text = string("Exception: ") + e.what();
    log_print(text);
}

std::runtime_error create_error(string const &what) {
    return std::runtime_error(what + ": " + strerror(errno));
}