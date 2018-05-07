//
// Created by josdas on 06.05.18.
//

#ifndef OS_UTILS_H
#define OS_UTILS_H

#include <string>
#include <sys/stat.h>

using std::string;


Path get_cur_path() {
    char buf[MAX_PATH_LEN];
    getcwd(buf, MAX_PATH_LEN);
    string str_path(buf);
    return Path(str_path);
}


struct stat_d {
    __mode_t st_mode;
    __blksize_t st_blksize;
    __ino_t st_ino;

    explicit stat_d(struct stat st)
            : st_mode(st.st_mode),
              st_blksize(st.st_blksize),
              st_ino(st.st_ino) {}

};


stat_d get_stat(string const &path) {
    struct stat dir_stat{};
    stat(path.c_str(), &dir_stat);
    return stat_d(dir_stat);
}


#endif //OS_UTILS_H
