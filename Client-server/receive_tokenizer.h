//
// Created by josdas on 21.05.18.
//

#ifndef CLIENT_SERVER_RECEIVE_TOKENIZER_H
#define CLIENT_SERVER_RECEIVE_TOKENIZER_H

#include <string>

#include "scoped_socket.h"


const size_t MAX_RECEIVE_ITERATION = 128;

class SocketTokenizer {
    std::string separator;
    std::string remainder;

public:
    explicit SocketTokenizer(std::string const &separator);

    void reset();

    std::string receive(ScopedSocket &socket);
};

#endif //CLIENT_SERVER_RECEIVE_TOKENIZER_H
