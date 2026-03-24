#pragma once

#include "constants.hpp"
#include <array>

struct Precomputed {

    std::array<bitmask, BoardSize> knightMoves{};
    std::array<bitmask, BoardSize> kingMoves{};

    Precomputed() {
        initialize();
    }

private:
    void initialize();
};