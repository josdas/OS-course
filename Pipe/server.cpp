//
// Created by josdas on 29.05.18.
//

#include <string>

#include "unix_socket.h"
#include "utils.h"
#include "pipe.h"


using std::string;


void run_server(const std::string &file) {
    ScopedUnixSocket server_socket;
    server_socket.bind(file);
    server_socket.listen();
    log_print("Server started successfully");
    while (true) {
        ScopedUnixSocket client_socket(server_socket.accept());
        log_print("Got new connection");

        ScopedPipe pipe;
        client_socket.send_fd(pipe.getFds()[0]);
        client_socket.send_fd(pipe.getFds()[1]);
        log_print("Sent fds of pipe to client");

        char c = pipe.receive();
        log_print("Got query: '" + std::to_string(c) + "'");

        char answer = c + 1;
        if (c >= 'a' && c <= 'z') {
            answer = c - 'a' + 'A';
        } else if (c >= 'A' && c <= 'Z') {
            answer = c - 'A' + 'a';
        }
        log_print("Answer: '" + std::to_string(answer) + "'");
        pipe.send(answer);
        log_print("Sent respond");
    }
}
