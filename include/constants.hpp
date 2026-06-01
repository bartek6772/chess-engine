#pragma once

#include <cassert>

constexpr int BoardLength = 8;
constexpr int BoardSize = BoardLength * BoardLength;

constexpr int MaxPiecesCount = 12;
constexpr int MaxSearchDepth = 64;

using bitmask = unsigned long long;

inline constexpr bitmask getFileMask(int file) {
    assert(file >= 0 && file <= 7);
    return 0x0101010101010101ULL << file;
}

inline constexpr bitmask getRankMask(int rank) {
    assert(file >= 0 && file <= 7);
    return 0x00000000000000FFULL << (rank * 8);
}

constexpr bitmask FILE_A = getFileMask(0);
constexpr bitmask FILE_H = getFileMask(7);

constexpr bitmask RANK_1 = getRankMask(0);
constexpr bitmask RANK_2 = getRankMask(1);
constexpr bitmask RANK_3 = getRankMask(2);
constexpr bitmask RANK_4 = getRankMask(3);
constexpr bitmask RANK_5 = getRankMask(4);
constexpr bitmask RANK_6 = getRankMask(5);
constexpr bitmask RANK_7 = getRankMask(6);
constexpr bitmask RANK_8 = getRankMask(7);
