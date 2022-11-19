#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

#include <thread>

#include <unistd.h>

#include "common.h"
#include "Game.h"

int main() {
    // class for socket is necessary.

    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock == -1) {
        throw std::runtime_error("Error while creating socket");
    }

    sockaddr_in sockaddrIn = {
        .sin_family = AF_INET,
        .sin_port = htons(12345)
    };

    sockaddrIn.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, reinterpret_cast<const sockaddr*>(&sockaddrIn), sizeof(sockaddrIn)) == -1) {
        close(sock);
        throw std::runtime_error("Error while binding");
    }

    std::thread accept_thread([&](){

    });

    close(sock);
    return 0;
}
