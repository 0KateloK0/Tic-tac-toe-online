#include "common.h"

const in_port_t net::Socket::port = htons(30001);
sockaddr_in net::Socket::address = {
    .sin_family = AF_INET,
    .sin_port = port,
    .sin_addr{.s_addr = INADDR_ANY}
};

net::Socket::Socket() : is_open(false) {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        throw std::runtime_error("Error while creating socket\n" + std::to_string(errno));
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
    int option = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int));

    if (bind(sock, reinterpret_cast<const sockaddr*>(&address), sizeof(address)) == -1) {
        throw std::runtime_error("Error while binding\n" + std::to_string(errno));
    }

    if (listen(sock, 50) == -1) {
        throw std::runtime_error("Error while listening\n" + std::to_string(errno));
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
        throw std::runtime_error("Error while creating socket\n" + std::to_string(errno));
    }

    if (connect(sock, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == -1) {
        throw std::runtime_error("Error while connecting\n" + std::to_string(errno));
    }

    is_open = true;
}

void net::TCPConnectionSocket::send_message(const std::string &msg) const {
    ssize_t amount = send(sock, msg.c_str(), msg.size(), 0);
    if (amount < 0) {
        throw std::runtime_error("Error while sending data:\n" + std::to_string(errno));
    }
}

std::string net::TCPConnectionSocket::get_message() const {
    char buffer[buffer_size + 1];
    ssize_t amount = recv(sock, buffer, buffer_size, 0);
    if (amount < 0) {
        throw std::runtime_error("Error while receiving data\n" + std::to_string(errno));
    }
    buffer[amount] = '\0';
    return {buffer};
}

net::TCPConnectionSocket::TCPConnectionSocket(int sock) : Socket(sock) {}

std::string net::etos(net::app_status status)  {
    return std::to_string(status) + "\n";
}