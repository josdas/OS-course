#include <iostream>
#include <vector>
#include <wait.h>
#include <unistd.h>
#include <memory>
#include <cstring>

using std::string;
using std::vector;
using std::cin;
using std::cout;
using std::cerr;
using std::getline;
using std::unique_ptr;


vector<string> parse_command(string const &command) {
    vector<string> result;
    string cur_string;
    for (char c : command) {
        if (isspace(c)) {
            if (!cur_string.empty()) {
                result.push_back(cur_string);
                cur_string.clear();
            }
        } else {
            cur_string.push_back(c);
        }
    }
    if (!cur_string.empty()) {
        result.push_back(cur_string);
    }
    return result;
}


unique_ptr<const char *const> ptr_from_vector_string(vector<string> const &argv) {
    size_t n = argv.size();
    const auto **ptr = new const char *[n + 1];
    ptr[n] = '\0';
    for (size_t i = 0; i < n; i++) {
        ptr[i] = argv[i].c_str();
    }
    return unique_ptr<const char *const>(ptr);
}


void execute(char **argv) {
    int status;
    pid_t pid = fork();
    switch (pid) {
        case -1:
            cerr << "Fork failed\n";
            break;
        case 0:
            if (execvp(argv[0], argv) < 0) {
                cerr << "Execute command failed\n";
                exit(-1);
            }
            break;
        default:
            if (waitpid(pid, &status, 0) == -1) {
                cerr << "Error occurred during execution\n";
            }
            break;
    }
}


void greeting() {
    cout << "~$ ";
    cout.flush();
}


void run_shell() {
    string command;
    greeting();
    while (getline(cin, command)) {
        if (command == "exit") {
            return;
        }
        vector<string> parsed_command = parse_command(command);
        auto ptr_argv = ptr_from_vector_string(parsed_command);
        execute(const_cast<char **>(ptr_argv.get()));
        greeting();
    }
}


int main(int argc, char **argv) {
    run_shell();
    return 0;
}