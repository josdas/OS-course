//
// Created by josdas on 29.05.18.
//

#ifndef PIPE_PIPE_H
#define PIPE_PIPE_H

class ScopedPipe {
    int fds[2];

public:
    ScopedPipe();

    ScopedPipe(int in, int out);

    ScopedPipe(ScopedPipe &&other) noexcept;

    ScopedPipe(ScopedPipe const &) = delete;

    ~ScopedPipe();

    ScopedPipe &operator=(ScopedPipe const &) = delete;

    ScopedPipe &operator=(ScopedPipe &&other) noexcept;

    void swap(ScopedPipe &other);


    void close();

    void send(char c);

    char receive();

    const int *getFds() const;
};

#endif //PIPE_PIPE_H
