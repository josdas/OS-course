//
// Created by josdas on 25.05.18.
//
#include <string>

#include "buffered_socket.h"
#include "utils.h"


using std::string;


BufferedSocket::BufferedSocket(int fd, std::string const &message_end)
        : socket(fd), message_end(message_end) {}

BufferedSocket::BufferedSocket(ScopedSocket &&socket, std::string const &separator)
        : socket(std::move(socket)), message_end(separator) {}


void BufferedSocket::send_form_buffer() {
    size_t n = std::min(buffer_write.length(), BUFFER_SIZE_);
    auto packet = buffer_write.substr(0, n);
    auto len = socket.send(packet);
    log_print("Sent part of message with length of " + std::to_string(len));
    buffer_write.erase(buffer_write.begin(), buffer_write.begin() + len);
}

void BufferedSocket::send_query(const std::string &data) {
    buffer_write += data + message_end;
    send_form_buffer();
}

std::string BufferedSocket::receive_query() {
    string mes = socket.receive();
    log_print("Got part of message with length of " + std::to_string(mes.length()));
    buffer_read += mes;
    auto pos = buffer_read.find(message_end);
    if (pos == std::string::npos) {
        return "";
    }
    string result = buffer_read.substr(0, pos);
    buffer_read = buffer_read.substr(pos + message_end.length());
    return result;
}

bool BufferedSocket::ready_to_write() const {
    return !buffer_write.empty();
}

const ScopedSocket &BufferedSocket::get_socket() const {
    return socket;
}
