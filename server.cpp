#include "common.h"
#include "Game.h"
#include <optional>

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





    std::cout << net::etos(net::GAME_STARTED) << '\n';

    net::AcceptingSocket master_socket;

    std::optional<net::TCPConnectionSocket> player1_sock = std::nullopt;
    std::optional<net::TCPConnectionSocket> player2_sock = std::nullopt;

    fd_set rdfs{};
    fd_set master_rdfs{};

    FD_SET(0, &master_rdfs); // stdin
    FD_SET(master_socket, &master_rdfs);

    while (true) {
        rdfs = master_rdfs;

        // finding max socket value for select (yep, this syscall is really weird)
        int mx = master_socket;
        if (player1_sock.has_value()) mx = std::max(mx, static_cast<int>(*player1_sock));
        if (player2_sock.has_value()) mx = std::max(mx, static_cast<int>(*player2_sock));

        // no timeout
        select(mx + 1, &rdfs, nullptr, nullptr, nullptr);

        // some input from stdin
        if (FD_ISSET(0, &rdfs)) {
            // just a nice way to kill server
            std::string buffer;
            std::cin >> buffer;
            if (buffer == "c") {
                return 0;
            }
        }

        // means there is some new connection (should be either first or second player, no more connections will be made)
        if (FD_ISSET(master_socket, &rdfs)) {
            // yes, this IS a pointer on std::optional, god bless C++
            auto *for_accept = player1_sock.has_value() ? &player2_sock : &player1_sock;
            *for_accept = net::TCPConnectionSocket(master_socket.accept_connection());

            FD_SET(**for_accept, &master_rdfs);

            if (player1_sock.has_value()) {
                player1_sock->send_message(etos(net::PLAYER1_CONNECTED) + "Wait for the second player to connect");
            }

            // if both players connected, a game can start, and the listener will be different
            if (player2_sock.has_value()) {
                player1_sock->send_message(etos(net::PLAYER2_CONNECTED) + "Both players connected. You are playing as marks");
                player2_sock->send_message(etos(net::PLAYER2_CONNECTED) + "Both players connected. You are playing as zeros");
                break;
            }
        }

        // I'm ignoring any messages from players, while the game hasn't started
    }


    Game game;

    player1_sock->send_message(etos(net::GAME_STARTED) + "Game has started, it's your turn\n" + game.to_string());
    player2_sock->send_message(etos(net::GAME_STARTED) + "Game has started, it's your opponent's turn\n" + game.to_string());

    bool is_game_ended = false;

    while (!is_game_ended) {
        rdfs = master_rdfs;

        // both players should have connected by now, otherwise my app will just die))))
        int mx = master_socket;
        mx = std::max(mx, static_cast<int>(*player1_sock));
        mx = std::max(mx, static_cast<int>(*player2_sock));

        select(mx + 1, &rdfs, nullptr, nullptr, nullptr);

        // some input from stdin
        if (FD_ISSET(0, &rdfs)) {
            // just a nice way to kill server
            std::string buffer;
            std::cin >> buffer;
            if (buffer == "c") {
                // both players should be notified that the server has broken down
                player1_sock->send_message(etos(net::SERVER_BROKE) + "Game ended, server broke down");
                player2_sock->send_message(etos(net::SERVER_BROKE) + "Game ended, server broke down");
                return 1;
            }
        }

        // DRY? No. Two almost identical player handlers? YES!
        if (FD_ISSET(*player1_sock, &rdfs)) {
            try {
                auto msg = player1_sock->get_message();
                if (msg.empty()) {
                    player2_sock->send_message(etos(net::PLAYER1_DISCONNECTED) + "Your opponent disconnected, game ended");
                    return 1;
                }
                int x, y;
                // yes, I was lazy to write an actual parser, is it that important here?))
                std::stringstream{msg} >> x >> y;
                // if inputs are incorrect, server will also break

                if (game.is_correct_move(x, y, true)) {
                    game.make_move(x, y);
                    player1_sock->send_message(etos(net::PLAYER1_MADE_MOVE) + game.to_string() + "it's your opponent's turn");
                    player2_sock->send_message(etos(net::PLAYER1_MADE_MOVE) + game.to_string() + "it's your turn");

                    switch (game.get_status()) {
                        case Game::MARK_WON:
                            player1_sock->send_message(etos(net::PLAYER1_WON) + "Congratulations, you've won!");
                            player2_sock->send_message(etos(net::PLAYER1_WON) + "Haha loser");
                            is_game_ended = true;
                            break;
                        case Game::DRAW:
                            player1_sock->send_message(etos(net::GAME_DRAWN) + "It's draw my dudes");
                            player2_sock->send_message(etos(net::GAME_DRAWN) + "It's draw my dudes");
                            is_game_ended = true;
                            break;
                        default:
                            break;
                    }
                }
                else {
                    player1_sock->send_message(etos(net::INCORRECT_MOVE) + "Incorrect move, or it's not your turn");
                }
            }
            catch (std::runtime_error const& error) {
                std::cerr << error.what() << '\n';
                player1_sock->send_message(etos(net::SERVER_BROKE) + "Game ended, server broke down");
                player2_sock->send_message(etos(net::SERVER_BROKE) + "Game ended, server broke down");
                return 1;
            }
        }

        if (FD_ISSET(*player2_sock, &rdfs)) {
            try {
                auto msg = player2_sock->get_message();
                if (msg.empty()) {
                    player1_sock->send_message(etos(net::PLAYER2_DISCONNECTED) + "Your opponent disconnected, game ended");
                    return 1;
                }
                int x, y;
                std::stringstream{msg} >> x >> y;
                if (game.is_correct_move(x, y, true)) {
                    game.make_move(x, y);
                    player2_sock->send_message(game.to_string());
                    player1_sock->send_message(etos(net::PLAYER2_MADE_MOVE) + game.to_string() + "it's your turn");
                    player2_sock->send_message(etos(net::PLAYER2_MADE_MOVE) + game.to_string() + "it's your opponent's turn");
                    switch (game.get_status()) {
                        case Game::ZERO_WON:
                            player1_sock->send_message(etos(net::PLAYER2_WON) + "Lmao how did you lose playing marks?");
                            player2_sock->send_message(etos(net::PLAYER2_WON) + "Wow that's an achievement");
                            is_game_ended = true;
                            break;
                        case Game::DRAW:
                            player1_sock->send_message(etos(net::GAME_DRAWN) + "It's draw my dudes");
                            player2_sock->send_message(etos(net::GAME_DRAWN) + "It's draw my dudes");
                            is_game_ended = true;
                            break;
                        default:
                            break;
                    }
                }
                else {
                    player2_sock->send_message(etos(net::INCORRECT_MOVE) + "Incorrect move, or it's not your turn");
                }
            }
            catch (std::runtime_error const& error) {
                std::cerr << error.what() << '\n';
                player1_sock->send_message(etos(net::SERVER_BROKE) + "Game ended, server broke down");
                player2_sock->send_message(etos(net::SERVER_BROKE) + "Game ended, server broke down");
                return 1;
            }
        }
    }

    player1_sock->send_message(etos(net::GAME_ENDED) + "Thanks for the game, disconnecting");
    player2_sock->send_message(etos(net::GAME_ENDED) + "Thanks for the game, disconnecting");

    return 0;
}
