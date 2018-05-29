//
// Created by josdas on 29.05.18.
//

#include <string>
#include <iostream>

#include "unix_socket.h"
#include "utils.h"
#include "pipe.h"
#include <unistd.h>


using std::string;
using std::cin;
using std::cout;
using std::endl;


void run_client(const std::string &file) {
    ScopedUnixSocket socket;
    socket.connect(file);
    cout << "Client connected successfully" << endl;

    int in, out;
    in = socket.receive_fd();
    try {
        out = socket.receive_fd();
    } catch (...) {
        close(in);
        throw;
    }
    ScopedPipe pipe(in, out);
    cout << "Got pipe from server" << endl;

    cout << "Enter one letter" << endl;
    char c;
    cin >> c;

    pipe.send(c);
    cout << "Sent: " << "'" << c << "'" << endl;
    cout << "Got: " << "'" << pipe.receive() << "'" << endl;
}
