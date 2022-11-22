#include "common.h"

net::Socket::~Socket() {
    if (is_created)
        close(socket_fd);
}

net::Socket::Socket() {
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    is_created = socket_fd != -1;
    if (!is_created)
        throw std::runtime_error("Error while creating socket");
}

net::AcceptingSocket::AcceptingSocket() {
    sockaddr_in sockaddrIn = {
        .sin_family = AF_INET,
        .sin_port = port,
        .sin_addr{.s_addr = INADDR_ANY}
    };
    if (bind(socket_fd, reinterpret_cast<const sockaddr*>(&sockaddrIn), sizeof(sockaddrIn)) == -1) {
        throw std::runtime_error("Error while binding");
    }

    if (listen(socket_fd, 50) == -1) {
        throw std::runtime_error("Error while listening");
    }
}

int net::AcceptingSocket::accept_connection() {
    sockaddr_in peer_addr {
            .sin_family = AF_INET,
            .sin_port = port,
            .sin_addr{.s_addr = INADDR_ANY}
    };
    socklen_t peer_addr_size = sizeof(peer_addr);
    return accept(socket_fd, reinterpret_cast<sockaddr*>(&peer_addr), &peer_addr_size);
}

std::string net::AcceptingSocket::get_message(int accepted_socket) {
    const size_t buffer_size = 1000;
    char* buffer = new char[buffer_size];
    ssize_t amount = recv(accepted_socket, buffer, buffer_size, MSG_WAITALL);
    buffer[amount] = '\0'; // just to be sure
    return {buffer};
}

net::ConnectingSocket::ConnectingSocket() {
    sockaddr_in sockaddrIn = {
        .sin_family = AF_INET,
        .sin_port = port,
        .sin_addr{.s_addr = INADDR_ANY}
    };

    if (connect(socket_fd, reinterpret_cast<sockaddr*>(&sockaddrIn), sizeof(sockaddrIn)) == -1) {
        throw std::runtime_error("Error while connecting");
    }
}

void net::ConnectingSocket::send_message(const std::string &msg) {
    ssize_t len = send(socket_fd, msg.data(), msg.size(), MSG_NOSIGNAL);
}
