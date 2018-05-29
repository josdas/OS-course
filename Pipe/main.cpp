#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "utils.h"
#include "server.h"
#include "client.h"


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
                "program <server or client> <name of file for UNIX socket>" << endl;
        return 1;
    }
    auto args = parse_argv(argc, argv);

    string mode = args[0];
    string file = args[1];
    if (mode == "server") {
        try {
            run_server(file);
        } catch (std::runtime_error const &e) {
            log_print(std::string("Error in server: ") + e.what());
            return 1;
        }
    } else if (mode == "client") {
        try {
            run_client(file);
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