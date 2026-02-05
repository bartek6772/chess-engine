#pragma once

#include "constants.hpp"
#include <array>

struct Precomputed {

    std::array<bitmask, BoardSize> knightMoves{};

    Precomputed() { initialize(); }

private:
    void initialize();
};