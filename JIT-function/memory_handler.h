//
// Created by josdas on 07.05.18.
//

#ifndef HW03_MEMORY_HANDLER_H
#define HW03_MEMORY_HANDLER_H

#include <sys/mman.h>
#include <cstdio>
#include <new>
#include <cstring>

using btype = unsigned char;

class MemoryHandler {
    size_t size_;
    int prot_;
    void *ptr_;

public:
    explicit MemoryHandler(size_t size, int prot = PROT_READ | PROT_WRITE, int flags = MAP_PRIVATE | MAP_ANONYMOUS)
            : size_(size), prot_(prot) {
        ptr_ = mmap(nullptr, size, prot_, flags, -1, 0);
        if (ptr_ == MAP_FAILED) {
            throw std::bad_alloc();
        }
    }

    ~MemoryHandler() {
        if (munmap(ptr_, size_) == -1) {
            throw std::bad_alloc();
        }
    }

    bool switch_type(int prot) {
        if (prot == prot_) {
            return true;
        }
        if (mprotect(ptr_, size_, prot) == -1) {
            return false;
        }
        prot_ = prot;
        return true;
    }

    void *get_mem() {
        return ptr_;
    }

    void set_mem(btype const *data, size_t data_size) {
        memcpy(ptr_, data, data_size);
    }
};

#endif //HW03_MEMORY_HANDLER_H
