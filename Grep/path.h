//
// Created by josdas on 06.05.18.
//

#ifndef OS_PATH_H
#define OS_PATH_H

#include <string>
#include <vector>

using std::string;
using std::vector;

const size_t MAX_PATH_LEN = 1 << 12;
const char *SEPARATE_IN_PATH = "/";


class Path {
    vector<string> path_;

    explicit Path(vector<string> path_) : path_(path_) {}

public:
    explicit Path(string const &str) : path_(1, str) {}

    Path(Path const &other) = default;

    Path() = default;

    string to_string() const {
        string result;
        for (size_t i = 0; i + 1 < path_.size(); i++) {
            result.append(path_[i] + SEPARATE_IN_PATH);
        }
        if (!path_.empty()) {
            result.append(path_.back());
        }
        return result;
    }

    Path get_relative_path() const {
        auto tmp = path_;
        tmp.erase(tmp.begin());
        return Path(tmp);
    }

    size_t length() const {
        return path_.size();
    }

    friend bool operator<(Path const &a, Path const &b) {
        if (a.length() != b.length()) {
            return a.length() < b.length();
        }
        return a.to_string() < b.to_string();
    }

    static Path add_to_path(Path const &path, string const &next) {
        Path tmp(path);
        tmp.path_.push_back(next);
        return tmp;
    }
};

#endif //OS_PATH_H
