//
// Created by josdas on 23.05.18.
//
#include <sys/epoll.h>
#include <stdexcept>
#include <unistd.h>

#include <vector>

#include "scoped_epoll.h"
#include "utils.h"


using std::vector;

ScopedEpoll::ScopedEpoll() {
    fd = epoll_create(EPOLL_SIZE);
    if (fd < 0) {
        throw create_error("Can not create epoll");
    }
}

ScopedEpoll::ScopedEpoll(int fd) : fd(fd) {}

ScopedEpoll::ScopedEpoll(ScopedEpoll &&other) noexcept : fd(other.fd) {
    other.fd = -1;
}

ScopedEpoll::~ScopedEpoll() {
    if (fd >= 0) {
        try {
            close();
        } catch (std::runtime_error const &e) {
            log_print(e);
        }
    }
}

ScopedEpoll &ScopedEpoll::operator=(ScopedEpoll &&other) noexcept {
    swap(other);
    return *this;
}

void ScopedEpoll::swap(ScopedEpoll &other) {
    std::swap(fd, other.fd);
}

void ScopedEpoll::close() {
    auto code = ::close(fd);
    if (code < 0) {
        throw create_error("Can not close epoll with file descriptor "
                           + std::to_string(fd));
    }
    fd = -1;
}

void ScopedEpoll::add_event(int fd, uint32_t events) {
    struct epoll_event event{};
    event.events = events;
    event.data.fd = fd;
    int code = epoll_ctl(ScopedEpoll::fd, EPOLL_CTL_ADD, fd, &event);
    if (code < 0) {
        throw create_error("Can not add event with file descriptor "
                           + std::to_string(fd));
    }
}

void ScopedEpoll::remove_event(int fd) {
    int code = epoll_ctl(ScopedEpoll::fd, EPOLL_CTL_DEL, fd, nullptr);
    if (code < 0) {
        throw create_error("Can not remove event with file descriptor "
                           + std::to_string(fd));
    }
}

vector<struct epoll_event> ScopedEpoll::wait() {
    vector<struct epoll_event> events(MAX_EVENTS);
    int count = epoll_wait(fd, events.data(), MAX_EVENTS, -1);
    if (count < 0) {
        throw create_error("Error occurred during waiting");
    }
    events.resize(static_cast<size_t>(count));
    return events;
}

