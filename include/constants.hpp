#pragma once

constexpr int BoardLength = 8;
constexpr int BoardSize = BoardLength * BoardLength;

using bitmask = unsigned long long;

constexpr bitmask FILE_A = 0x0101010101010101;
constexpr bitmask FILE_H = 0x8080808080808080;

constexpr bitmask RANK_1 = 0x00000000000000FF;
constexpr bitmask RANK_4 = 0x00000000FF000000;
constexpr bitmask RANK_5 = 0x000000FF00000000;
constexpr bitmask RANK_8 = 0xFF00000000000000;
