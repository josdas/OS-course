//
// Created by josdas on 21.05.18.
//

#include <string>
#include <vector>
#include <iostream>

#include "client.h"
#include "scoped_socket.h"
#include "utils.h"
#include "receive_tokenizer.h"


using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::cin;


void run_client(uint32_t address, uint16_t port) {
    ScopedSocket socket;
    socket.connect(address, port);

    cout << "Examples:\n"
            "show info ru-en\n"
            "show db\n"
            "define sym-en 1\n"
            "define * star\n"
            "define ! star\n"
            "quit\n" << endl;

    SocketTokenizer tokenizer("\r\n");

    cout << "Waiting for connection to " << address << " with port " << port << endl;
    cout << "'" + tokenizer.receive(socket) + "'" << endl;
    cout << "Connected" << endl;

    while (true) {
        cout << "Enter your query" << endl;
        string query;
        getline(cin, query);
        send(socket, query);
        while (true) {
            string s = tokenizer.receive(socket);
            if (s.empty() || s == "end") {
                break;
            }
            cout << "'" + s + "'" << endl;
        }
        cout << "Finished getting respond for query '" + query + "'" << endl;
    }
}
