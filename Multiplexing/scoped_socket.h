//
// Created by josdas on 21.05.18.
//

#ifndef CLIENT_SERVER_SCOPED_SOCKET_H
#define CLIENT_SERVER_SCOPED_SOCKET_H

#include <cstdint>
#include <string>


class ScopedSocket {
    int fd;

public:
    ScopedSocket();

    explicit ScopedSocket(int fd) noexcept;

    ScopedSocket(ScopedSocket &&other) noexcept;

    ScopedSocket(ScopedSocket const &) = delete;

    virtual ~ScopedSocket();

    ScopedSocket &operator=(ScopedSocket const &) = delete;

    ScopedSocket &operator=(ScopedSocket &&other) noexcept;

    void swap(ScopedSocket &other);


    void close();

    void connect(uint32_t address, uint16_t port);

    std::string receive();

    ssize_t send(const std::string &data);

    void bind(uint16_t port);

    void listen();

    int accept();

    int get_fd() const;
};

#endif //CLIENT_SERVER_SCOPED_SOCKET_H
