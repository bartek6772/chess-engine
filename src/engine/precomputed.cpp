#include "precomputed.hpp"
#include "bitboard.hpp"
#include "constants.hpp"
#include "square.hpp"
#include "utility.hpp"
#include <array>

void precomputeKnightMoves(std::array<Bitboard, BoardSize>& knightMoves) {
    constexpr int MovesCount = 8;
    static std::array<int, MovesCount> move_row{ 2, 2, 1, -1, -2, -2, -1, 1 };
    static std::array<int, MovesCount> move_col{ -1, 1, 2, 2, 1, -1, -2, -2 };

    for (int row = 0; row < BoardLength; row++) {
        for (int col = 0; col < BoardLength; col++) {

            Bitboard moves;
            for (int i = 0; i < MovesCount; i++) {
                int r = row + move_row[i];
                int c = col + move_col[i];

                if (r >= 0 && r < BoardLength && c >= 0 && c < BoardLength) {
                    moves |= Bitboard(Square(c, r));
                }
            }

            knightMoves[Square(col, row)] = moves;
        }
    }
}

void precomputeKingMoves(std::array<Bitboard, BoardSize>& kingMoves) {
    constexpr int MovesCount = 8;
    static std::array<int, MovesCount> move_row{ 1, 1, 0, -1, -1, -1, 0, 1 };
    static std::array<int, MovesCount> move_col{ 0, 1, 1, 1, 0, -1, -1, -1 };

    for (int row = 0; row < BoardLength; row++) {
        for (int col = 0; col < BoardLength; col++) {

            Bitboard moves;
            for (int i = 0; i < MovesCount; i++) {
                int r = row + move_row[i];
                int c = col + move_col[i];

                if (r >= 0 && r < BoardLength && c >= 0 && c < BoardLength) {
                    moves |= Bitboard(Square(c, r));
                }
            }

            kingMoves[Square(col, row)] = moves;
        }
    }
}

void Precomputed::initialize() {
    precomputeKnightMoves(knightMoves);
    precomputeKingMoves(kingMoves);
}