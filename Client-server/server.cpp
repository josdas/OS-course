//
// Created by josdas on 21.05.18.
//

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <algorithm>

#include "server.h"
#include "scoped_socket.h"
#include "utils.h"
#include "receive_tokenizer.h"


using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::unordered_map;
using std::pair;


using dict_t = unordered_map<string, string>;
using db_t = unordered_map<string, dict_t>;

const char *CONNECT_MESSAGE = "success";
const char *MESSAGE_END = "\r\n";
const size_t MAX_QUERY_COUNT = 16;


void define_query(db_t &db, ScopedSocket &socket, string const &dict_name, string const &word) {
    if (dict_name != "*" && dict_name != "!") {
        if (db.count(dict_name) == 0) {
            send(socket, "Invalid database");
            send(socket, "end");
            return;
        }
        dict_t &cur_dict = db[dict_name];
        if (cur_dict.count(word) == 0) {
            send(socket, "No match");
            send(socket, "end");
            return;
        }
        send(socket, "1 definitions");
        send(socket, cur_dict[word] + " from " + dict_name);
        send(socket, "end");
        return;
    }
    if (dict_name == "!") {
        vector<std::pair<string, string> > definitions;
        for (auto const &dict: db) {
            auto it = dict.second.find(word);
            if (it != dict.second.end()) {
                definitions.emplace_back(dict.first, it->second);
            }
        }
        if (definitions.empty()) {
            send(socket, "No match");
            send(socket, "end");
            return;
        }
        send(socket, std::to_string(definitions.size()) + " definitions");
        for (auto const &answer: definitions) {
            send(socket, answer.second + " from " + answer.first);
        }
        send(socket, "end");
        return;
    }
    if (dict_name == "*") {
        for (auto const &dict: db) {
            auto it = dict.second.find(word);
            if (it != dict.second.end()) {
                send(socket, "1 definitions");
                send(socket, it->second + " from " + dict.first);
                send(socket, "end");
                return;
            }
        }
        send(socket, "No match");
        send(socket, "end");
        return;
    }
}

void show_db(db_t &db, ScopedSocket &socket) {
    if (db.empty()) {
        send(socket, "No databases");
        send(socket, "end");
        return;
    }
    string answer;
    answer += std::to_string(db.size()) + string(" databases: ");
    for (auto const &dict: db) {
        answer += " " + dict.first + ",";
    }
    answer.pop_back();
    send(socket, answer);
    send(socket, "end");
}


void show_info(db_t &db, ScopedSocket &socket, string const &name) {
    if (db.count(name) == 0) {
        send(socket, "Invalid database");
        return;
    }
    send(socket, "name: " + name + ", size: " + std::to_string(db[name].size()));
    send(socket, "end");
}

bool process_query(db_t &db, ScopedSocket &socket, vector<string> const &query) {
    string str_q = "QUERY: ";
    for (auto const &qu : query) {
        str_q += qu + "|";
    }
    log_print(str_q);

    if (query.size() == 1 && query[0] == "quit") {
        send(socket, "Connection is closed");
        send(socket, "end");
        return false;
    }
    if (query.size() == 3 && query[0] == "define") {
        define_query(db, socket, query[1], query[2]);
        return true;
    }
    if (query.size() == 2 && query[0] == "show" && (query[1] == "db" || query[1] == "databases")) {
        show_db(db, socket);
        return true;
    }
    if (query.size() == 3 && query[0] == "show" && query[1] == "info") {
        show_info(db, socket, query[2]);
        return true;
    }
    send(socket, "Command is not implemented");
    send(socket, "end");
    return true;
}

db_t load_db(string const &path) {
    std::ifstream file(path);
    string line;
    db_t db;
    while (getline(file, line)) {
        const auto tokens = tokenize(line, '|');
        for (auto const &def : tokens) {
            const auto parsed = tokenize(def, '=');
            if (parsed.size() == 3) {
                db[parsed[0]][parsed[1]] = parsed[2];
            } else {
                throw std::runtime_error("Incorrect db format " + path);
            }
        }
    }
    return db;
}

void run_server(uint16_t port) {
    ScopedSocket listener;
    listener.bind(port);
    listener.listen();

    SocketTokenizer tokenizer(MESSAGE_END);

    db_t db = load_db("db.txt");
    log_print("Loaded " + std::to_string(db.size()) + " dicts");
    log_print("Server started successfully");

    while (true) {
        try {
            auto socket = listener.accept();
            send(socket, CONNECT_MESSAGE);
            log_print("Got new connection");

            tokenizer.reset();
            bool stop = false;
            for (int query_count = 0; !stop && query_count < MAX_QUERY_COUNT;) {
                auto data = tokenizer.receive(socket);
                if (data.empty()) {
                    continue;
                }
                log_print("Got '" + data + "'");
                std::transform(data.begin(), data.end(), data.begin(), ::tolower);
                const auto parsed = tokenize(data, ' ');
                stop = !process_query(db, socket, parsed);
                log_print("Processed");
                query_count++;
            }
            log_print("Quota expired or connection lost");
        } catch (std::runtime_error const &e) {
            log_print(e);
        }
    }
}
