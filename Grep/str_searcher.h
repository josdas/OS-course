//
// Created by josdas on 06.05.18.
//

#ifndef OS_STR_SEARCHER_H
#define OS_STR_SEARCHER_H

#include <string>
#include <vector>
#include <cassert>


using std::string;
using std::vector;


class StringSearcherInStream {
    string pattern;
    vector<size_t> prefix_fun;
    size_t last_prefix_value;

    size_t prefix_iterate(size_t ind, char c) const {
        assert(ind <= prefix_fun.size());
        if (ind == prefix_fun.size()) {
            ind = prefix_fun[ind - 1];
        }
        while (ind > 0 && c != pattern[ind]) {
            ind = prefix_fun[ind - 1];
        }
        if (c == pattern[ind]) {
            ind++;
        }
        return ind;
    }

public:
    explicit StringSearcherInStream(string pattern_)
            : pattern(std::move(pattern_)), prefix_fun(pattern.size()), last_prefix_value(0) {
        for (size_t i = 1; i < pattern.size(); i++) {
            size_t start = prefix_fun[i - 1];
            prefix_fun[i] = prefix_iterate(start, pattern[i]);
        }
    }

    bool append_to_stream(char c) {
        last_prefix_value = prefix_iterate(last_prefix_value, c);
        return last_prefix_value == pattern.size();
    }

    void reset_stream() {
        last_prefix_value = 0;
    }
};

#endif //OS_STR_SEARCHER_H
