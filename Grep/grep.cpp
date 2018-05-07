#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <sys/stat.h>
#include <algorithm>

#include "path.h"
#include "autoclosable.h"
#include "str_searcher.h"
#include "utils.h"

using std::string;
using std::vector;
using std::cin;
using std::cout;
using std::cerr;
using std::pair;
using std::set;
using std::map;
using std::endl;
using std::ostream;


void print_error(std::ostream &error_stream, string const &error) {
    error_stream << error << endl;
}


bool check_contains(string const &path, string const &str, ostream &error_stream) {
    auto fd = AutoCloseableFile(path, O_RDONLY);
    if (fd.get() < 0) {
        print_error(error_stream, "Can't open file: " + path);
        return false;
    }

    ssize_t chars_read;

    stat_d file_stat = get_stat(path);
    size_t buffer_size = static_cast<size_t>(file_stat.st_blksize);
    char buffer[buffer_size];

    StringSearcherInStream searcher(str);

    while (true) {
        chars_read = read(fd.get(), buffer, buffer_size);
        if (chars_read <= 0) {
            break;
        }
        for (ssize_t i = 0; i < chars_read; i++) {
            if (searcher.append_to_stream(buffer[i])) {
                return true;
            }
        }
    }
    if (chars_read < 0) {
        print_error(error_stream, "Error during reading file: " + path);
        return false;
    }

    return false;
}


vector<string> grep(string const &str, ostream &error_stream) {
    Path start_path = get_cur_path();
    ino_t start_ino = get_stat(start_path.to_string()).st_ino;

    map<ino_t, Path> distance;
    distance[start_ino] = start_path;

    set<pair<Path, ino_t> > heap;
    heap.insert({start_path, start_ino});

    set<ino_t> good_inos;
    set<ino_t> checked;

    while (!heap.empty()) {
        auto begin_it = heap.begin();
        auto path = begin_it->first;
        heap.erase(begin_it);

        auto cur_dir = AutoCloseableDir(path.to_string());
        if (cur_dir.get() == nullptr) {
            print_error(error_stream, "Error: can't open directory: " + path.to_string());
            continue;
        }

        dirent *entry = nullptr;
        while (true) {
            entry = readdir(cur_dir.get());
            if (entry == nullptr) {
                break;
            }

            string next_name(entry->d_name);
            if (next_name == ".." || next_name == ".") {
                continue;
            }

            Path next_path = Path::add_to_path(path, next_name);
            stat_d next_stat = get_stat(next_path.to_string());
            ino_t next_ino = next_stat.st_ino;
            __mode_t mode = next_stat.st_mode;


            Path previous_path = next_path;
            auto it_path = distance.find(next_ino);
            if (it_path == distance.end()) {
                distance[next_ino] = next_path;
            } else {
                if (path < it_path->second) {
                    it_path->second = next_path;
                } else {
                    previous_path = it_path->second;
                }
            }

            if (S_ISDIR(mode)) {
                auto last_it = heap.find({previous_path, next_ino});
                if (last_it != heap.end()) {
                    heap.erase(last_it);
                }
                heap.insert({next_path, next_ino});
            } else if (S_ISREG(mode)) {
                bool is_checked = checked.count(next_ino) > 0;
                if (!is_checked) {
                    if (check_contains(next_path.to_string(), str, error_stream)) {
                        good_inos.insert(next_ino);
                    }
                    checked.insert(next_ino);
                }
            }

        }
        cur_dir.close();
    }
    vector<Path> paths;
    for (auto const &ino : good_inos) {
        Path const &path = distance[ino].get_relative_path();
        paths.push_back(path);
    }
    std::sort(paths.begin(), paths.end());

    vector<string> result;
    for (auto const &path : paths) {
        result.push_back(path.to_string());
    }
    return result;
}


int main(int argc, const char *argv[]) {
    if (argc != 2) {
        print_error(cerr, "One argument is required.\n"
                "Example: ./grep aaa");
        return 1;
    }
    string goal(argv[1]);
    auto results = grep(goal, cerr);
    for (auto const &str : results) {
        cout << str << endl;
    }
    return 0;
}