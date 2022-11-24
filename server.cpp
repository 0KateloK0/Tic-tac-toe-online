#include "common.h"
#include "Game.h"
#include <optional>
#include <sstream>

int main() {
    /*net::AcceptingSocket master_socket;

    std::vector<std::optional<net::ConnectingSocket>> sockets;

    fd_set rfds{};

    Game game;

    int max_sd;
    std::string buffer(1024, 0);

        while(true) {
            // making fd_set
            FD_ZERO(&rfds);
            FD_SET(0, &rfds); // stdin
            FD_SET(master_socket.socket_fd, &rfds);
            max_sd = master_socket.socket_fd;
            for (auto const& sock: sockets) {
                FD_SET(sock->socket_fd, &rfds);
                max_sd = std::max(sock->socket_fd, max_sd);
            }

            select(max_sd + 1, &rfds, nullptr, nullptr, nullptr);

            if (FD_ISSET(0, &rfds)) {
                std::cin >> buffer;
                if (buffer == "c") {
                    break;
                }
            }

            if (FD_ISSET(master_socket.socket_fd, &rfds)) {
                sockets.emplace_back();
                sockets.back()->socket_fd = master_socket.accept_connection();
            }

            for (int i = 0; i < sockets.size(); ++i) {
                if (!FD_ISSET(sockets[i]->socket_fd, &rfds))
                    continue;
                auto msg = master_socket.get_message(sockets[i]->socket_fd);
                if (msg.size() <= 0) {
                    close(sockets[i]->socket_fd);
                    FD_CLR(sockets[i]->socket_fd, &rfds);
                    sockets.erase(sockets.begin() + i);
                    i = 0;
                    continue;
                }

                std::stringstream source{msg};
                int x, y;
                source >> x >> y;
                if (!game.is_correct_move(x, y)) {
                    std::string m{"Incorrect move"};
                    send(sockets[i]->socket_fd, m.data(), m.size(), 0);
                }
                else {

                }
            }
        }

    for (auto const& sock: sockets) {
        close(sock->socket_fd);
    }*/

//    close(master_socket);

//    net::AcceptingSocket master_socket;


    // accepting both players loop

    net::AcceptingSocket master_socket;



    return 0;
}
