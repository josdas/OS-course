#include <iostream>
#include <cstring>
#include <utility>
#include <vector>
#include <memory>
#include <sstream>
#include "memory_handler.h"
#include "jit_function.h"

using std::vector;
using std::unique_ptr;
using std::string;
using std::endl;


const btype CODE[] = {
        0x55,
        0x48, 0x89, 0xe5,
        0x89, 0x7d, 0xfc,
        0x8b, 0x45, 0xfc,
        0x83, 0xc0, 0x05,
        0x5d,
        0xc3
};

const size_t CHANGE_POS = sizeof(CODE) - 3;


void print_error(string const &str) {
    std::cerr << str << endl;
}


std::pair<string, btype> parse(string const &command) {
    std::istringstream stream(command);
    string query;
    btype value;
    stream >> query >> value;
    return {query, value};
};


void help() {
    std::cout << "This is a program for calculating the sum of an argument with a constant char in memory." << endl
              << "The function constant is 5 at the start." << endl
              << "You can execute the function with the command 'calc <one char argument>'" << endl
              << "Example: 'calc 0'" << endl
              << "You can change the constant in the function with the command 'change <one char argument>''" << endl
              << "Example: 'change &'" << endl;
}


void run_console_handler(unique_ptr<JitFunction> const &jitFunction) {
    string command;
    while (std::getline(std::cin, command)) {
        if (command == "--help" || command == "-h") {
            help();
            continue;
        }
        auto temp = parse(command);
        string query = temp.first;
        btype value = temp.second;

        if (query == "calc") {
            auto result = jitFunction->calc<btype>(value);
            std::cout << "Result: '" << result << "'" << endl;
        } else if (query == "change") {
            jitFunction->change_code(CHANGE_POS, value);
            std::cout << "Value changed to '" << value << "'" << endl;
        } else {
            print_error("Unknown command\nTry '--help' for more information.");
        }
    }
}


int main() {
    vector<btype> code(CODE, CODE + sizeof(CODE));
    unique_ptr<JitFunction> jitFunction;

    try { // Safe create JitFunction
        jitFunction = std::make_unique<JitFunction>(code);
    } catch (std::bad_alloc const &e) {
        print_error("Can't allocate memory");
        return 1;
    }

    run_console_handler(jitFunction);

    try { // Safe delete JitFunction
        jitFunction.reset(nullptr);
    } catch (std::bad_alloc const &e) {
        print_error("Can't deallocate memory");
        return 1;
    }
    return 0;
}