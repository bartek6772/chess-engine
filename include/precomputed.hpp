#pragma once

#include "bitboard.hpp"
#include "constants.hpp"
#include <array>

struct Precomputed {

    std::array<Bitboard, BoardSize> knightMoves{};
    std::array<Bitboard, BoardSize> kingMoves{};

    Precomputed() {
        initialize();
    }

private:
    void initialize();
};