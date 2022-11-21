#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <sys/un.h>

#include <thread>

#include <unistd.h>

#include "common.h"
#include "Game.h"

int main() {
    std::ios_base::sync_with_stdio(false);
    // class for socket is necessary.

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == -1) {
        throw std::runtime_error("Error while creating socket");
    }

    /*sockaddr_un addr = {
        .sun_family = AF_UNIX,
        .sun_path
    };*/

    sockaddr_in sockaddrIn = {
        .sin_family = AF_INET,
        .sin_port = htons(12345),
        .sin_addr{.s_addr = INADDR_ANY}
    };

    if (bind(sock, reinterpret_cast<const sockaddr*>(&sockaddrIn), sizeof(sockaddrIn)) == -1) {
        close(sock);
        throw std::runtime_error("Error while binding");
    }

    if (listen(sock, 50) == -1) {
        close(sock);
        throw std::runtime_error("Error while listening");
    }

    std::thread accept_thread([&](){
//        while (true) {
            sockaddr_in peer_addr {
                    .sin_family = AF_INET,
                    .sin_port = htons(12345),
                    .sin_addr{.s_addr = INADDR_ANY}
            };
            socklen_t peer_addr_size = sizeof(peer_addr);
            int accepted_socket = accept(sock, reinterpret_cast<sockaddr*>(&peer_addr), &peer_addr_size);
//            if (accepted_socket == -1) continue;
            const size_t buffer_size = 1000;
            char* buffer = new char[buffer_size];
            ssize_t amount = recv(accepted_socket, buffer, buffer_size, MSG_WAITALL);
            buffer[amount] = '\0'; // just to be sure
            std::cout << buffer << std::endl;
            delete [] buffer;
//        }
    });

    accept_thread.join();

    /*std::thread waitingLoop([]() {
        while(true) {}
    });*/

//    waitingLoop.join();

    close(sock);
    return 0;
}
