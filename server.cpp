#include "common.h"
#include "Game.h"
#include <optional>

int main() {
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
            int sock = master_socket.accept_connection();

            if (player1_sock.has_value())
                player2_sock.emplace(sock);
            else
                player1_sock.emplace(sock);

            if (player1_sock.has_value() && !player2_sock.has_value()) {
                player1_sock->send_message(etos(net::PLAYER1_CONNECTED) + "Wait for the second player to connect\n");
            }

            // if both players connected, a game can start, and the listener will be different
            if (player2_sock.has_value()) {
                player1_sock->send_message(etos(net::PLAYER2_CONNECTED) + "Both players connected. You are playing as marks\n");
                player2_sock->send_message(etos(net::PLAYER2_CONNECTED) + "Both players connected. You are playing as zeros\n");
                break;
            }
        }

        // I'm ignoring any messages from players, while the game hasn't started
    }

    FD_SET(*player1_sock, &master_rdfs);
    FD_SET(*player2_sock, &master_rdfs);

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
                player1_sock->send_message(etos(net::SERVER_BROKE) + "Game ended, server broke down\n");
                player2_sock->send_message(etos(net::SERVER_BROKE) + "Game ended, server broke down\n");
                return 1;
            }
        }

        // DRY? No. Two almost identical player handlers? YES!
        if (FD_ISSET(*player1_sock, &rdfs)) {
            try {
                auto msg = player1_sock->get_message();
                if (msg.empty()) {
                    player2_sock->send_message(etos(net::PLAYER1_DISCONNECTED) + "Your opponent disconnected, game ended\n");
                    return 1;
                }
                int x, y;
                // yes, I was lazy to write an actual parser, is it that important here?))
                std::stringstream{msg} >> x >> y;
                // if inputs are incorrect, server will also break

                if (game.is_correct_move(x, y, true)) {
                    auto state = game.make_move(x, y, true);
                    player1_sock->send_message(etos(net::PLAYER1_MADE_MOVE) + game.to_string() + "it's your opponent's turn\n");
                    player2_sock->send_message(etos(net::PLAYER1_MADE_MOVE) + game.to_string() + "it's your turn\n");

                    switch (state) {
                        case Game::MARK_WON:
                            player1_sock->send_message(etos(net::PLAYER1_WON) + "Congratulations, you've won!\n");
                            player2_sock->send_message(etos(net::PLAYER1_WON) + "Haha loser\n");
                            is_game_ended = true;
                            break;
                        case Game::DRAW:
                            player1_sock->send_message(etos(net::GAME_DRAWN) + "It's draw my dudes\n");
                            player2_sock->send_message(etos(net::GAME_DRAWN) + "It's draw my dudes\n");
                            is_game_ended = true;
                            break;
                        default:
                            break;
                    }
                }
                else {
                    player1_sock->send_message(etos(net::INCORRECT_MOVE) + "Incorrect move, or it's not your turn\n");
                }
            }
            catch (std::runtime_error const& error) {
                std::cerr << error.what() << '\n';
                player1_sock->send_message(etos(net::SERVER_BROKE) + "Game ended, server broke down\n");
                player2_sock->send_message(etos(net::SERVER_BROKE) + "Game ended, server broke down\n");
                return 1;
            }
        }

        if (FD_ISSET(*player2_sock, &rdfs)) {
            try {
                auto msg = player2_sock->get_message();
                if (msg.empty()) {
                    player1_sock->send_message(etos(net::PLAYER2_DISCONNECTED) + "Your opponent disconnected, game ended\n");
                    return 1;
                }
                int x, y;
                std::stringstream{msg} >> x >> y;
                if (game.is_correct_move(x, y, false)) {
                    auto state = game.make_move(x, y, false);
                    player1_sock->send_message(etos(net::PLAYER2_MADE_MOVE) + game.to_string() + "it's your turn\n");
                    player2_sock->send_message(etos(net::PLAYER2_MADE_MOVE) + game.to_string() + "it's your opponent's turn\n");
                    switch (state) {
                        case Game::ZERO_WON:
                            player1_sock->send_message(etos(net::PLAYER2_WON) + "Lmao how did you lose playing marks?\n");
                            player2_sock->send_message(etos(net::PLAYER2_WON) + "Wow that's an achievement\n");
                            is_game_ended = true;
                            break;
                        case Game::DRAW:
                            player1_sock->send_message(etos(net::GAME_DRAWN) + "It's draw my dudes\n");
                            player2_sock->send_message(etos(net::GAME_DRAWN) + "It's draw my dudes\n");
                            is_game_ended = true;
                            break;
                        default:
                            break;
                    }
                }
                else {
                    player2_sock->send_message(etos(net::INCORRECT_MOVE) + "Incorrect move, or it's not your turn\n");
                }
            }
            catch (std::runtime_error const& error) {
                std::cerr << error.what() << '\n';
                player1_sock->send_message(etos(net::SERVER_BROKE) + "Game ended, server broke down\n");
                player2_sock->send_message(etos(net::SERVER_BROKE) + "Game ended, server broke down\n");
                return 1;
            }
        }
    }

    player1_sock->send_message(etos(net::GAME_ENDED) + "Thanks for the game, disconnecting\n");
    player2_sock->send_message(etos(net::GAME_ENDED) + "Thanks for the game, disconnecting\n");

    return 0;
}
