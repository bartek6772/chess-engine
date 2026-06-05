#pragma once

#include "bitboard.hpp"
#include <cassert>

constexpr int BoardLength = 8;
constexpr int BoardSize = BoardLength * BoardLength;

constexpr int MaxPiecesCount = 12;
constexpr int MaxSearchDepth = 64;
constexpr int MaxEnginePLY = 2 * MaxSearchDepth;

inline constexpr Bitboard getFileMask(int file) {
    assert(file >= 0 && file <= 7);
    return Bitboard(0x0101010101010101ULL << file);
}

inline constexpr Bitboard getRankMask(int rank) {
    assert(rank >= 0 && rank <= 7);
    return Bitboard(0x00000000000000FFULL << (rank * 8));
}

constexpr Bitboard FILE_A = getFileMask(0);
constexpr Bitboard FILE_H = getFileMask(7);

constexpr Bitboard RANK_1 = getRankMask(0);
constexpr Bitboard RANK_2 = getRankMask(1);
constexpr Bitboard RANK_3 = getRankMask(2);
constexpr Bitboard RANK_4 = getRankMask(3);
constexpr Bitboard RANK_5 = getRankMask(4);
constexpr Bitboard RANK_6 = getRankMask(5);
constexpr Bitboard RANK_7 = getRankMask(6);
constexpr Bitboard RANK_8 = getRankMask(7);
