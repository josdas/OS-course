//
// Created by josdas on 21.05.18.
//

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

#include <stdexcept>
#include <string>

#include "scoped_socket.h"
#include "utils.h"


using std::string;
using std::runtime_error;


ScopedSocket::ScopedSocket() {
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        throw create_error("Can not create socket");
    }
    int optval = 1;
    int code = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if (code == -1) {
        close();
        throw create_error("Can not set up socket options");
    }
}

ScopedSocket::ScopedSocket(int fd) noexcept : fd(fd) {}

ScopedSocket::ScopedSocket(ScopedSocket &&other) noexcept : fd(other.fd) {
    other.fd = -1;
}

ScopedSocket::~ScopedSocket() {
    if (fd >= 0) {
        try {
            close();
        } catch (runtime_error const &e) {
            log_print(e);
        }
    }
}

ScopedSocket &ScopedSocket::operator=(ScopedSocket &&other) noexcept {
    swap(other);
    return *this;
}

void ScopedSocket::swap(ScopedSocket &other) {
    std::swap(fd, other.fd);
}

void ScopedSocket::close() {
    auto code = ::close(fd);
    if (code < 0) {
        throw create_error("Can not close socket with file descriptor "
                           + std::to_string(fd));
    }
    fd = -1;
}

void ScopedSocket::connect(uint32_t address, uint16_t port) {
    sockaddr_in socket_address{};
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(port);
    socket_address.sin_addr.s_addr = address;

    int code = ::connect(fd, reinterpret_cast<sockaddr *>(&socket_address), sizeof(socket_address));
    if (code < 0) {
        throw create_error("Can not create to address " + std::to_string(address)
                           + " with port " + std::to_string(port));
    }
}

string ScopedSocket::recieve() {
    string result(BUFFER_SIZE_, ' ');
    ssize_t bytes_read = ::read(fd, (void *) (result.data()), BUFFER_SIZE_);
    if (bytes_read == -1) {
        throw create_error("Error occurred during receiving from socket (fd "
                           + std::to_string(fd) + ")");
    }
    result.resize(static_cast<size_t>(bytes_read));
    return result;
}

void ScopedSocket::send(const std::string &data) {
    size_t sent = 0;
    while (sent < data.size()) {
        char const *start = data.data() + sent;
        size_t n = data.size() - sent;
        auto now_sent = write(fd, start, n);
        if (now_sent < 0) {
            throw create_error("Error occurred during sending from socket "
                                       "(fd " + std::to_string(fd) + ")");
        }
        sent += now_sent;
    }
}

void ScopedSocket::bind(uint16_t port) {
    sockaddr_in address{};

    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;

    int code = ::bind(fd, reinterpret_cast<sockaddr *>(&address), sizeof(address));
    if (code < 0) {
        throw create_error("Can not bind socket to port "
                           + std::to_string(port));
    }
}

void ScopedSocket::listen() {
    int code = ::listen(fd, 0);
    if (code < 0) {
        throw create_error("Can not listen socket");
    }
}

ScopedSocket ScopedSocket::accept() {
    int nfd = ::accept(fd, nullptr, nullptr);
    if (nfd < 0) {
        throw create_error("Can not accept");
    }
    return ScopedSocket(nfd);
}
