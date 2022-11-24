#include "Game.h"

bool Game::is_correct_move(size_t x, size_t y, bool player) const {
    return (player == is_mark_move) && field[y][x] == EMPTY;
}

Game::status Game::make_move(size_t x, size_t y, bool player) {
    if (is_correct_move(x, y, player)) {
        history.push_back(field);
        field[y][x] = is_mark_move ? MARK : ZERO;
        Game::status state = get_status();
        is_mark_move = !is_mark_move;
        return state;
    }
    return Game::IS_GOING;
}

std::string Game::to_string() const {
    std::string res;
    res += std::string(WIDTH * 2 + 1, '-');
    res += '\n';
    for (auto it: field) {
        res += '|';
        for (auto jit : it) {
            switch (jit) {
            case EMPTY:
                res += ' ';
                break;
            case MARK:
                res += 'x';
                break;
            case ZERO:
                res += 'o';
                break;
            }
            res += '|';
        }
        res += '\n';
        res += std::string(WIDTH * 2 + 1, '-');
        res += '\n';
    }
    return res;
}

Game::status Game::get_status() const {
    int counter = 0;
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            if ((field[i][j] == MARK && is_mark_move) || (field[i][j] == ZERO && !is_mark_move))
                ++counter;
            else
                counter = 0;
        }
        if (counter == 3)
            return is_mark_move ? MARK_WON : ZERO_WON;
    }

    counter = 0;
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            if ((field[j][i] == MARK && is_mark_move) || (field[j][i] == ZERO && !is_mark_move))
                ++counter;
            else
                counter = 0;
        }
        if (counter == 3)
            return is_mark_move ? MARK_WON : ZERO_WON;
    }

    counter = 0;
    for (int i = 0; i < WIDTH; ++i) {
        if ((field[i][i] == MARK && is_mark_move) || (field[i][i] == ZERO && !is_mark_move))
            ++counter;
    }
    if (counter == 3) return is_mark_move ? MARK_WON : ZERO_WON;

    counter = 0;
    for (int i = 0; i < WIDTH; ++i) {
        if ((field[i][WIDTH - 1 - i] == MARK && is_mark_move) || (field[i][WIDTH - 1 - i] == ZERO && !is_mark_move))
            ++counter;
    }
    if (counter == 3) return is_mark_move ? MARK_WON : ZERO_WON;

    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            if (field[i][j] == EMPTY)
                return IS_GOING;
        }
    }

    return DRAW;
}
