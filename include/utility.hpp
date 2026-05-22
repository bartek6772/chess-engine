#pragma once

#include "constants.hpp"
#include <bit>

inline constexpr bitmask setBit(int bit) {
    return 1ULL << bit;
}

inline constexpr int rankOf(int square) {
    return square >> 3;
}

inline constexpr int fileOf(int square) {
    return square & 7;
}

inline constexpr int makeSquare(int file, int rank) {
    return file + rank * BoardLength;
}

inline int readBit(bitmask& mask) {
    int bit = std::countr_zero(mask);
    mask &= (mask - 1);
    return bit;
}