//
// Created by josdas on 21.05.18.
//

#include <netinet/in.h>

#include <exception>
#include <string>
#include <iostream>
#include <cstring>
#include <vector>

#include "utils.h"
#include "scoped_socket.h"


using std::string;
using std::cerr;
using std::endl;
using std::vector;


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

uint32_t string_to_address(std::string const &str) {
    int ad[4] = {};
    int ret = std::sscanf(str.data(), "%d.%d.%d.%d",
                          &ad[0], &ad[1], &ad[2], &ad[3]);
    if (ret != 4) {
        throw std::runtime_error("Invalid address");
    }
    uint32_t reverse_address = 0;
    for (size_t i = 0; i < 4; ++i) {
        if (ad[i] < 0 || ad[i] > 255) {
            throw std::runtime_error("Invalid address");
        }
        auto cur_byte = static_cast<uint32_t>(ad[i]);
        reverse_address += (cur_byte << (8 * (3 - i)));
    }
    uint32_t address = htonl(reverse_address);
    return address;
}

vector<string> tokenize(std::string const &str, char sep) {
    string cur_substr;
    vector<string> tokens;
    for (char c : str) {
        if (c == sep) {
            if (!cur_substr.empty()) {
                tokens.push_back(cur_substr);
                cur_substr.clear();
            }
        } else {
            cur_substr.push_back(c);
        }
    }
    if (!cur_substr.empty()) {
        tokens.push_back(cur_substr);
    }
    return tokens;
}


void send(ScopedSocket &socket, string const &message) {
    log_print("Sending '" + message + "'");
    socket.send(message + "\r\n");
}