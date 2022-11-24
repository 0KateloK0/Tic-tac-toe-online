#include "common.h"

const in_port_t net::Socket::port = htons(12345);
sockaddr_in net::Socket::address = {
    .sin_family = AF_INET,
    .sin_port = port,
    .sin_addr{.s_addr = INADDR_ANY}
};

net::Socket::Socket() : is_open(false) {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        throw std::runtime_error("Error while creating socket");
    }
    is_open = true;
}

net::Socket::~Socket() {
    if (is_open) {
        close(sock);
    }
}

net::Socket::operator int() const {
    return sock;
}

net::Socket::Socket(int sock) : sock(sock), is_open(true) {}

net::AcceptingSocket::AcceptingSocket() : Socket() {
    if (bind(sock, reinterpret_cast<const sockaddr*>(&address), sizeof(address)) == -1) {
        throw std::runtime_error("Error while binding");
    }

    if (listen(sock, 50) == -1) {
        throw std::runtime_error("Error while listening");
    }
}

int net::AcceptingSocket::accept_connection() const {
    sockaddr_in peer_addr = address;
    socklen_t peer_addr_size = sizeof(peer_addr);
    return accept(sock, reinterpret_cast<sockaddr*>(&peer_addr), &peer_addr_size);
}


net::TCPConnectionSocket::TCPConnectionSocket() : Socket() {
    is_open = false;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        throw std::runtime_error("Error while creating socket");
    }

    if (connect(sock, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == -1) {
        throw std::runtime_error("Error while connecting");
    }

    is_open = true;
}

void net::TCPConnectionSocket::send_message(const std::string &msg) const {
    ssize_t amount = send(sock, msg.data(), msg.size(), 0);
    if (amount < 0) {
        throw std::runtime_error("Error while sending data");
    }
}

std::string net::TCPConnectionSocket::get_message() const {
    std::array<char, buffer_size> buffer{};
    ssize_t amount = recv(sock, buffer.data(), buffer_size, 0);
    if (amount < 0) {
        throw std::runtime_error("Error while receiving data");
    }
    buffer[amount] = '\0';
    return {buffer.data()};
}
