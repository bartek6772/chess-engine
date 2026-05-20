#pragma once

constexpr int BoardLength = 8;
constexpr int BoardSize = BoardLength * BoardLength;

constexpr int MaxPiecesCount = 12;
constexpr int MaxSearchDepth = 64;

using bitmask = unsigned long long;

constexpr bitmask FILE_A = 0x0101010101010101;
constexpr bitmask FILE_H = 0x8080808080808080;

constexpr bitmask RANK_1 = 0x00000000000000FF;
constexpr bitmask RANK_4 = 0x00000000FF000000;
constexpr bitmask RANK_5 = 0x000000FF00000000;
constexpr bitmask RANK_8 = 0xFF00000000000000;

// NOLINTBEGIN
namespace Squares {
constexpr bitmask A1 = 0;
constexpr bitmask B1 = 1;
constexpr bitmask C1 = 2;
constexpr bitmask D1 = 3;
constexpr bitmask E1 = 4;
constexpr bitmask F1 = 5;
constexpr bitmask G1 = 6;
constexpr bitmask H1 = 7;

constexpr bitmask A8 = 56;
constexpr bitmask B8 = 57;
constexpr bitmask C8 = 58;
constexpr bitmask D8 = 59;
constexpr bitmask E8 = 60;
constexpr bitmask F8 = 61;
constexpr bitmask G8 = 62;
constexpr bitmask H8 = 63;
} // namespace Squares
// NOLINTEND
