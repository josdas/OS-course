#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "client.h"
#include "server.h"
#include "utils.h"


using std::vector;
using std::string;
using std::cout;
using std::endl;


vector<string> parse_argv(int argc, char *argv[]) {
    assert(argc >= 1);
    vector<string> args(static_cast<size_t>(argc) - 1);
    for (size_t i = 1; i < argc; i++) {
        args[i - 1] = string(argv[i]);
    }
    return args;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Program mode is required\n"
                "Example:\n"
                "program server 8888" << endl;
        return 1;
    }
    auto args = parse_argv(argc, argv);

    const string mode = args[0];
    if (mode == "server") {
        if (args.size() < 2) {
            cout << "At least two arguments are required for server\n"
                    "Example:\n"
                    "program server 8888" << endl;
            return 0;
        }
        auto port = static_cast<uint16_t>(std::stoi(args[1]));
        try {
            run_server(port);
        } catch (std::runtime_error const &e) {
            log_print(std::string("Error in server: ") + e.what());
            return 1;
        }
    } else if (mode == "client") {
        if (args.size() < 3) {
            cout << "At least three arguments are required for client\n"
                    "Example:\n"
                    "program client 127.0.0.1 8888" << endl;
            return 1;
        }
        uint32_t address;
        try {
            address = string_to_address(args[1]);
        } catch (std::runtime_error const &e) {
            cout << "Valid IPv4 address is required for client as the second argument\n"
                    "Example:\n"
                    "program client 127.0.0.1 8888" << endl;
            return 1;
        }

        auto port = static_cast<uint16_t>(std::stoi(args[2]));
        try {
            run_client(address, port);
        } catch (std::runtime_error const &e) {
            log_print(std::string("Error in client: ") + e.what());
            return 1;
        }
    } else {
        cout << "Unknown mode" << endl;
        return 1;
    }
    return 0;
}