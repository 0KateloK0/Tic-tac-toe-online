#ifndef HW_SOCKET_GAME_H
#define HW_SOCKET_GAME_H

#include <iostream>
#include <array>
#include <vector>

class Game {
public:
    Game() = default;
    ~Game() = default;
    bool is_correct_move(size_t x, size_t y, bool player) const;
    enum status {
        IS_GOING,
        MARK_WON,
        ZERO_WON,
        DRAW
    };
    status make_move(size_t x, size_t y, bool player);
    status get_status() const;
    std::string to_string() const;
private:
    bool is_mark_move = true;

    static const size_t WIDTH = 3;
    static const size_t HEIGHT = 3;
    enum cell_type {
        EMPTY,
        MARK, // крестик
        ZERO // нолик
    };
    using field_type = std::array<std::array<cell_type, HEIGHT>, WIDTH>;
    field_type field{};

    std::vector<field_type> history;
};


#endif //HW_SOCKET_GAME_H
