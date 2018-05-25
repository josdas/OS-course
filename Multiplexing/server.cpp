//
// Created by josdas on 21.05.18.
//
#include <sys/epoll.h>

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <deque>

#include "server.h"
#include "buffered_socket.h"
#include "scoped_epoll.h"
#include "utils.h"


using std::string;
using std::vector;
using std::deque;
using std::unordered_map;


class ClientHandler {
    BufferedSocket socket;
    deque<string> queries;

public:
    explicit ClientHandler(int fd)
            : socket(fd, MESSAGE_END) {}

    bool receive() {
        string query = socket.receive_query();
        if (!query.empty()) {
            log_print("Got query '" + query
                      + "' from client with file descriptor "
                      + std::to_string(socket.get_socket().get_fd()));
            queries.push_back(query);
            return true;
        }
        return false;
    }

    bool respond() {
        if (queries.empty()) {
            if (socket.ready_to_write()) {
                socket.send_form_buffer();
                return true;
            }
            return false;
        }
        auto query = *queries.begin();
        queries.pop_front();
        auto respond = "Hello, " + query;
        send(socket, respond);
        return true;
    }
};

void run_server(uint16_t port) {
    ScopedSocket listener;
    listener.bind(port);
    listener.listen();

    unordered_map<int, ClientHandler> clients;

    ScopedEpoll epoll;
    epoll.add_event(listener.get_fd(), EPOLLIN);

    log_print("Server started successfully");

    while (true) {
        try {
            const auto events = epoll.wait();
            for (auto const &event : events) {
                try {
                    if (event.data.fd == listener.get_fd()) {
                        int socket_fd = listener.accept();
                        clients.emplace(std::make_pair(socket_fd, ClientHandler(socket_fd)));
                        epoll.add_event(socket_fd, EPOLLOUT | EPOLLIN | EPOLLERR | EPOLLRDHUP);
                        log_print("Got new connection to client with file descriptor "
                                  + std::to_string(socket_fd));
                    } else if (event.events & (EPOLLERR | EPOLLRDHUP)) {
                        int fd = event.data.fd;
                        log_print("Closing client with file descriptor " + std::to_string(fd));
                        epoll.remove_event(fd);
                        clients.erase(fd);
                    } else {
                        int fd = event.data.fd;
                        auto it = clients.find(fd);
                        if (it == clients.end()) {
                            throw create_error("Unknown client " + std::to_string(fd));
                        }
                        ClientHandler &client = it->second;
                        if (event.events & EPOLLIN) {
                            log_print("Client with file descriptor " + std::to_string(fd)
                                      + " is ready for writing");
                            client.receive();
                        }
                        if (event.events & EPOLLOUT) {
                            if (client.respond()) {
                                log_print("Client with file descriptor " + std::to_string(fd)
                                          + " is ready for reading");
                            }
                        }
                    }
                } catch (std::runtime_error const &e) {
                    log_print(e);
                }
            }
        } catch (std::runtime_error const &e) {
            log_print(e);
        }
    }
}
