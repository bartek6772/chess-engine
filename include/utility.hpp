#pragma once

#include "constants.hpp"

inline bitmask setBit(int bit) {
    return 1ULL << bit;
}

inline int rankOf(int square) {
    return square >> 3;
}

inline int fileOf(int square) {
    return square & 7;
}