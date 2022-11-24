#include "common.h"

int main () {
    net::TCPConnectionSocket sock;

    fd_set rdfs{};

    while (true) {
        FD_ZERO(&rdfs);
        FD_SET(0, &rdfs);
        FD_SET(sock, &rdfs);

        select(sock + 1, &rdfs, nullptr, nullptr, nullptr);

        if (FD_ISSET(0, &rdfs)) {
            std::string buffer;
            sock.send_message(buffer);
        }

        if (FD_ISSET(sock, &rdfs)) {
            char status;
            std::stringstream source{sock.get_message()};
            source >> status;
            auto state = static_cast<net::app_status>(status);
            std::cout << source.rdbuf() << std::flush;
            switch (state) {
                case net::PLAYER1_DISCONNECTED:
                case net::PLAYER2_DISCONNECTED:
                case net::SERVER_BROKE:
                case net::GAME_ENDED:
                    return 1;
                default:
                    break;
            }
        }
    }

    return 0;
}