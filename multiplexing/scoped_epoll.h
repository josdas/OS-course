//
// Created by josdas on 23.05.18.
//

#ifndef CLIENT_SERVER_SCOPED_EPOLL_H
#define CLIENT_SERVER_SCOPED_EPOLL_H

#include <vector>


const size_t EPOLL_SIZE = 1024;
const size_t MAX_EVENTS = 16;

class ScopedEpoll {
    int fd;

public:
    explicit ScopedEpoll();

    explicit ScopedEpoll(int fd);

    ScopedEpoll(ScopedEpoll &&other) noexcept;

    ScopedEpoll(ScopedEpoll const &) = delete;

    ~ScopedEpoll();

    ScopedEpoll &operator=(ScopedEpoll const &) = delete;

    ScopedEpoll &operator=(ScopedEpoll &&other) noexcept;

    void swap(ScopedEpoll &other);


    void close();

    void add_event(int fd, uint32_t events);

    void remove_event(int fd);

    std::vector<struct epoll_event> wait();
};

#endif //CLIENT_SERVER_SCOPED_EPOLL_H
