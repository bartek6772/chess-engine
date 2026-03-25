#include "precomputed.hpp"
#include "constants.hpp"
#include <array>

void precomputeKnightMoves(std::array<bitmask, BoardSize>& knightMoves) {
    constexpr int MovesCount = 8;
    static std::array<int, MovesCount> move_row{ 2, 2, 1, -1, -2, -2, -1, 1 };
    static std::array<int, MovesCount> move_col{ -1, 1, 2, 2, 1, -1, -2, -2 };

    for (int row = 0; row < BoardLength; row++) {
        for (int col = 0; col < BoardLength; col++) {

            bitmask moves = 0LL;
            for (int i = 0; i < MovesCount; i++) {
                int r = row + move_row[i];
                int c = col + move_col[i];

                if (r >= 0 && r < BoardLength && c >= 0 && c < BoardLength) {
                    moves |= 1ULL << (r * BoardLength + c);
                }
            }

            int square = row * BoardLength + col;
            knightMoves[square] = moves;
        }
    }
}

void precomputeKingMoves(std::array<bitmask, BoardSize>& kingMoves) {
    constexpr int MovesCount = 8;
    static std::array<int, MovesCount> move_row{ 1, 1, 0, -1, -1, -1, 0, 1 };
    static std::array<int, MovesCount> move_col{ 0, 1, 1, 1, 0, -1, -1, -1 };

    for (int row = 0; row < BoardLength; row++) {
        for (int col = 0; col < BoardLength; col++) {

            bitmask moves = 0LL;
            for (int i = 0; i < MovesCount; i++) {
                int r = row + move_row[i];
                int c = col + move_col[i];

                if (r >= 0 && r < BoardLength && c >= 0 && c < BoardLength) {
                    moves |= 1ULL << (r * BoardLength + c);
                }
            }

            int square = row * BoardLength + col;
            kingMoves[square] = moves;
        }
    }
}

void Precomputed::initialize() {
    precomputeKnightMoves(knightMoves);
    precomputeKingMoves(kingMoves);
}