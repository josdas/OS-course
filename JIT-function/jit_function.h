//
// Created by josdas on 07.05.18.
//

#ifndef HW03_JIT_FUNCTION_H
#define HW03_JIT_FUNCTION_H

#include "memory_handler.h"
#include <cassert>
#include <vector>

using std::vector;


class JitFunction {
    vector<btype> code;
    MemoryHandler memoryHandler;

public:
    explicit JitFunction(vector<btype> const &code)
            : code(code), memoryHandler(code.size()) {
        memoryHandler.set_mem(code.data(), code.size());
    }

    template<typename R, typename... Args>
    R calc(Args const &...args) {
        memoryHandler.switch_type(PROT_EXEC | PROT_READ);
        auto function = reinterpret_cast<R(*)(Args...)>(memoryHandler.get_mem());
        return function(args...);
    }

    void change_code(size_t position, btype value) {
        assert(position < code.size());
        memoryHandler.switch_type(PROT_WRITE | PROT_READ);
        code[position] = value;
        memoryHandler.set_mem(code.data(), code.size());
    }
};

#endif //HW03_JIT_FUNCTION_H
