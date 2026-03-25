#pragma once

#include "move.hpp"
#include <array>

struct MoveList {
    std::array<Move, 256> moves;
    int count = 0;

    void push(Move move) {
        moves[count++] = move;
    }

    int size() const {
        return count;
    }

    Move* begin() {
        return moves.begin();
    }

    Move* end() {
        return moves.begin() + count;
    }

    const Move* begin() const {
        return moves.begin();
    }

    const Move* end() const {
        return moves.begin() + count;
    }
};