//
// Created by josdas on 21.05.18.
//

#include <string>

#include "scoped_socket.h"
#include "receive_tokenizer.h"


using std::string;


SocketTokenizer::SocketTokenizer(std::string const &separator) : separator(separator) {}


void SocketTokenizer::reset() {
    remainder.clear();
}

std::string SocketTokenizer::receive(ScopedSocket &socket) {
    size_t sep_pos = string::npos;
    for (size_t i = 0; i < MAX_RECEIVE_ITERATION; i++) {
        sep_pos = remainder.find(separator);
        if (sep_pos != string::npos) {
            break;
        }
        string tmp = socket.recieve();
        if (tmp.empty()) {
            break;
        }
        remainder += tmp;
    }
    if (sep_pos == string::npos) {
        return "";
    }
    string result = remainder.substr(0, sep_pos);
    remainder = remainder.substr(sep_pos + separator.length());
    return result;
}
