//
// Created by josdas on 06.05.18.
//

#ifndef OS_AUTOCLOSABLE_H
#define OS_AUTOCLOSABLE_H

#include <string>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

using std::string;

class AutoCloseableDir {

    DIR *dir;

public:
    explicit AutoCloseableDir(string const &dir)
            : dir(opendir(dir.c_str())) {}

    void close() {
        if (dir != nullptr) {
            closedir(dir);
            dir = nullptr;
        }
    }

    DIR *get() {
        return dir;
    }

    ~AutoCloseableDir() {
        close();
    }
};


class AutoCloseableFile {
    int fd;

public:
    explicit AutoCloseableFile(string const &path, int flag)
            : fd(open(path.c_str(), flag)) {}

    void close_() {
        if (fd >= 0) {
            close(fd);
            fd = -1;
        }
    }

    int get() {
        return fd;
    }

    ~AutoCloseableFile() {
        close_();
    }
};

#endif //OS_AUTOCLOSABLE_H
