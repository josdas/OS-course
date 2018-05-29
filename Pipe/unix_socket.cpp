//
// Created by josdas on 29.05.18.
//
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>

#include <stdexcept>
#include <string>

#include "unix_socket.h"
#include "utils.h"


using std::string;
using std::runtime_error;


ScopedUnixSocket::ScopedUnixSocket() {
    socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        throw create_error("Can not create socket");
    }
    int optval = 1;
    int code = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if (code == -1) {
        close();
        throw create_error("Can not set up socket options");
    }
}

ScopedUnixSocket::ScopedUnixSocket(int fd) noexcept : socket_fd(fd) {}

ScopedUnixSocket::ScopedUnixSocket(ScopedUnixSocket &&other) noexcept : socket_fd(other.socket_fd) {
    other.socket_fd = -1;
}

ScopedUnixSocket::~ScopedUnixSocket() {
    if (socket_fd >= 0) {
        try {
            close();
        } catch (runtime_error const &e) {
            log_print(e);
        }
    }
}

ScopedUnixSocket &ScopedUnixSocket::operator=(ScopedUnixSocket &&other) noexcept {
    swap(other);
    return *this;
}

void ScopedUnixSocket::swap(ScopedUnixSocket &other) {
    std::swap(socket_fd, other.socket_fd);
}

void ScopedUnixSocket::close() {
    auto code = ::close(socket_fd);
    if (code < 0) {
        throw create_error("Can not close socket with file descriptor "
                           + std::to_string(socket_fd));
    }
    socket_fd = -1;
}

void ScopedUnixSocket::connect(std::string const &file) {
    sockaddr_un socket_address{.sun_family = AF_UNIX};
    strcpy(socket_address.sun_path, file.c_str());
    auto len = sizeof(socket_address.sun_family) + file.length();
    int code = ::connect(socket_fd, reinterpret_cast<sockaddr *>(&socket_address), len);
    if (code < 0) {
        throw create_error("Can not connect to file " + file);
    }
}

void ScopedUnixSocket::bind(std::string const &file) {
    sockaddr_un socket_address{.sun_family = AF_UNIX};
    strcpy(socket_address.sun_path, file.c_str());
    auto len = sizeof(socket_address.sun_family) + file.length();
    int code = ::bind(socket_fd, reinterpret_cast<sockaddr *>(&socket_address), len);
    if (code < 0) {
        throw create_error("Can not bind socket to file " + file);
    }
}

void ScopedUnixSocket::listen() {
    int code = ::listen(socket_fd, 0);
    if (code < 0) {
        throw create_error("Can not listen socket");
    }
}

int ScopedUnixSocket::accept() {
    int nfd = ::accept(socket_fd, nullptr, nullptr);
    if (nfd < 0) {
        throw create_error("Can not accept");
    }
    return nfd;
}

int ScopedUnixSocket::get_fd() const {
    return socket_fd;
}

void ScopedUnixSocket::send_fd(int fd) {
    struct msghdr msg = {nullptr};
    char buf[CMSG_SPACE(sizeof(fd))];
    memset(buf, 0, sizeof(buf));

    struct iovec io = { .iov_base = (void *) "", .iov_len = 1 };

    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);

    struct cmsghdr * cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(fd));

    memmove(CMSG_DATA(cmsg), &fd, sizeof(fd));

    msg.msg_controllen = cmsg->cmsg_len;

    if (sendmsg(socket_fd, &msg, 0) < 0) {
        throw create_error("Failed to send fd");
    }
}

int ScopedUnixSocket::receive_fd() {
    msghdr msg = {nullptr};
    cmsghdr *structcmsghdr;

    char buffer[CMSG_SPACE(sizeof(int))];
    char duplicate[512];

    bzero(buffer, sizeof(buffer));

    iovec io = {
            .iov_base = &duplicate,
            .iov_len = sizeof(duplicate)
    };

    msg.msg_control = buffer;
    msg.msg_controllen = sizeof(buffer);
    msg.msg_iov = &io;
    msg.msg_iovlen = 1;

    auto code = recvmsg(socket_fd, &msg, 0);
    if (code < 0) {
        throw create_error("Failed to receive fd");
    }

    structcmsghdr = CMSG_FIRSTHDR(&msg);
    int received_fd;
    memcpy(&received_fd, (int *) CMSG_DATA(structcmsghdr), sizeof(int));
    return received_fd;

}
