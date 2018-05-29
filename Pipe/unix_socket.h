//
// Created by josdas on 29.05.18.
//

#ifndef PIPE_UNIX_SOCKET_H
#define PIPE_UNIX_SOCKET_H

#include <string>


class ScopedUnixSocket {
    int socket_fd;

public:
    ScopedUnixSocket();

    explicit ScopedUnixSocket(int fd) noexcept;

    ScopedUnixSocket(ScopedUnixSocket &&other) noexcept;

    ScopedUnixSocket(ScopedUnixSocket const &) = delete;

    ~ScopedUnixSocket();

    ScopedUnixSocket &operator=(ScopedUnixSocket const &) = delete;

    ScopedUnixSocket &operator=(ScopedUnixSocket &&other) noexcept;

    void swap(ScopedUnixSocket &other);


    void close();

    void connect(std::string const &file);

    void bind(std::string const &file);

    void listen();

    int accept();

    int get_fd() const;


    void send_fd(int fd);

    int receive_fd();
};

#endif //PIPE_UNIX_SOCKET_H
