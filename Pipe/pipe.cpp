//
// Created by josdas on 29.05.18.
//

#include <unistd.h>

#include <stdexcept>

#include "pipe.h"
#include "utils.h"


using std::runtime_error;


ScopedPipe::ScopedPipe() {
    if (pipe(fds) < 0) {
        throw create_error("Can not create pipe");
    }
}

ScopedPipe::ScopedPipe(int in, int out) {
    fds[0] = in;
    fds[1] = out;
}

ScopedPipe::ScopedPipe(ScopedPipe &&other) noexcept {
    for (size_t i = 0; i < 2; i++) {
        fds[i] = other.fds[i];
        other.fds[i] = -1;
    }
}

ScopedPipe::~ScopedPipe() {
    if (fds[0] >= 0) {
        try {
            close();
        } catch (runtime_error const &e) {
            log_print(e);
        }
    }
}

ScopedPipe &ScopedPipe::operator=(ScopedPipe &&other) noexcept {
    swap(other);
    return *this;
}

void ScopedPipe::swap(ScopedPipe &other) {
    for (size_t i = 0; i < 2; i++) {
        std::swap(fds[i], other.fds[i]);
    }
}

void ScopedPipe::close() {
    int error_fd = -1;
    for (int &fd : fds) {
        if (fd >= 0) {
            auto code = ::close(fd);
            if (code < 0) {
                error_fd = fd;
            }
            fd = -1;
        }
    }
    if (error_fd >= 0) {
        throw create_error("Can not close pipe with file descriptor "
                           + std::to_string(error_fd));
    }
}

void ScopedPipe::send(char c) {
    size_t received = 0;
    uint8_t buffer[1];
    buffer[0] = static_cast<uint8_t>(c);
    while (received < 1) {
        ssize_t count = write(fds[1], buffer, sizeof(buffer));
        if (count < 0) {
            throw create_error("Can not send");
        }
        received += count;
    }
}

char ScopedPipe::receive() {
    size_t received = 0;
    uint8_t buffer[1];
    while (received < 1) {
        ssize_t count = read(fds[0], buffer, sizeof(buffer));
        if (count < 0) {
            throw create_error("Can not receive");
        }
        received += count;
    }
    return static_cast<char>(buffer[0]);
}

const int *ScopedPipe::getFds() const {
    return fds;
}
