#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

#include <thread>

#include <unistd.h>

int main () {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == -1) {
        throw std::runtime_error("Error while creating socket");
    }

    sockaddr_in sockaddrIn = {
            .sin_family = AF_INET,
            .sin_port = htons(12345),
            .sin_addr{.s_addr = INADDR_ANY}
    };

    if (connect(sock, reinterpret_cast<sockaddr*>(&sockaddrIn), sizeof(sockaddrIn)) == -1) {
        std::cerr << "Cringe";
        return -1;
    }


    std::string buffer{"Hello, world!"};
    int len = send(sock, buffer.data(), buffer.size(), MSG_NOSIGNAL);
    std::cout << len;

    close(sock);
    return 0;
}