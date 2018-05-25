//
// Created by josdas on 25.05.18.
//

#ifndef CLIENT_SERVER_BUFFERED_SCOPED_SOCKET_H
#define CLIENT_SERVER_BUFFERED_SCOPED_SOCKET_H

#include <string>

#include "scoped_socket.h"


class BufferedSocket {
    ScopedSocket socket;
    std::string buffer_read;
    std::string buffer_write;
    std::string message_end;

public:
    BufferedSocket(int fd, std::string const &message_end);

    BufferedSocket(ScopedSocket &&socket, std::string const &separator);

    const ScopedSocket &get_socket() const;

    bool ready_to_write() const;

    void send_form_buffer();

    void send_query(const std::string &data);

    std::string receive_query();
};

#endif //CLIENT_SERVER_BUFFERED_SCOPED_SOCKET_H
