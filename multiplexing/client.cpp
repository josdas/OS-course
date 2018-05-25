//
// Created by josdas on 21.05.18.
//

#include <sys/epoll.h>
#include <zconf.h>

#include <string>
#include <vector>
#include <deque>
#include <iostream>

#include "buffered_socket.h"
#include "client.h"
#include "scoped_socket.h"
#include "utils.h"
#include "scoped_epoll.h"


using std::string;
using std::vector;
using std::deque;
using std::cout;
using std::endl;
using std::cin;


void run_client(uint32_t address, uint16_t port) {
    cout << "Example:\n"
            "2\none\ntwo" << endl;
    cout << "Write -1 for autotesting with big strings" << endl;

    deque<string> queries;
    int n;
    cin >> n;
    if (n >= 0) {
        cin.ignore();
        for (int i = 0; i < n; i++) {
            string query;
            getline(cin, query);
            queries.push_back(query);
        }
    } else {
        queries.emplace_back(100000, 'z');
        queries.emplace_back(15000, 'a');
        for (int i = 0; i < 10; i++) {
            queries.emplace_back(3000, 'b' + i);
        }
    }
    cout << "Queries:" << endl;
    for (auto const &query : queries) {
        cout << "'" + query + "'" << endl;
    }

    ScopedSocket temp_socket;
    temp_socket.connect(address, port);
    BufferedSocket bsocket(std::move(temp_socket), MESSAGE_END);

    ScopedEpoll epoll;
    epoll.add_event(bsocket.get_socket().get_fd(), EPOLLOUT | EPOLLIN | EPOLLERR | EPOLLRDHUP);

    cout << "Connected to " << address << " with port " << port << endl;

    int got = 0;
    int sent = 0;
    int total = static_cast<int>(queries.size());

    while (true) {
        try {
            const auto events = epoll.wait();
            for (auto const &event : events) {
                try {
                    if (event.events & (EPOLLERR | EPOLLRDHUP)) {
                        cout << "Closed connection with server" << endl;
                        return;
                    } else {
                        if (event.events & EPOLLIN) {
                            cout << "Server is ready for writing" << endl;
                            const auto respond = bsocket.receive_query();
                            if (!respond.empty()) {
                                cout << "Got respond '" << respond << "'" << endl;
                                got++;
                                cout << "Got: " << got << " / Sent: " << sent << " / Total: " << total << endl;
                            }
                        }
                        if (event.events & EPOLLOUT) {
                            if (!queries.empty()) {
                                cout << "Sending new query" << endl;
                                const auto query = *queries.begin();
                                queries.pop_front();
                                send(bsocket, query);
                                sent++;
                            } else if (bsocket.ready_to_write()) {
                                cout << "Sending query from buffer" << endl;
                                bsocket.send_form_buffer();
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
