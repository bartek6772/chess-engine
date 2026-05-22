#pragma once

#include "constants.hpp"
#include <bit>
#include <cassert>

inline constexpr bitmask setBit(int bit) {
    assert(bit >= 0 && bit < 64);
    return 1ULL << bit;
}

inline constexpr int rankOf(int square) {
    assert(bit >= 0 && bit < 64);
    return square >> 3;
}

inline constexpr int fileOf(int square) {
    assert(bit >= 0 && bit < 64);
    return square & 7;
}

inline constexpr int makeSquare(int file, int rank) {
    assert(file >= 0 && file <= 7 && rank >= 0 && rank <= 7);
    return file + rank * BoardLength;
}

inline int readBit(bitmask& mask) {
    assert(mask != 0);
    int bit = std::countr_zero(mask);
    mask &= (mask - 1);
    return bit;
}