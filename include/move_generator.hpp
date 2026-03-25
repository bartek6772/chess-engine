#pragma once

#include "board.hpp"
#include "constants.hpp"
#include "magics.hpp"
#include "move.hpp"
#include "precomputed.hpp"
#include <vector>

// TODO: Precomputed can be embeded in MoveGenerator, removing the need to pass it in constructor

class MoveGenerator {
private:
    Precomputed precomputed;
    Magics magics;

    bitmask white_attacks;
    bitmask black_attacks;

    void generateAttacks(const Board& board);
    bitmask generateSideAttacks(const Board& board, int color);
    bool isSquareAttacked(const Board& board, int square);

    void generateKnightMoves(const Board& board, std::vector<Move>& moves, int color) const;
    void generateRookMoves(const Board& board, std::vector<Move>& moves, int color) const;
    void generateBishopMoves(const Board& board, std::vector<Move>& moves, int color) const;
    void generateQueenMoves(const Board& board, std::vector<Move>& moves, int color) const;
    void generatePawnMoves(const Board& board, std::vector<Move>& moves, int color) const;
    void generateKingMoves(const Board& board, std::vector<Move>& moves, int color) const;

public:
    auto generateMoves(const Board& board) -> std::vector<Move>;
    auto generateLegalMoves(Board& board) -> std::vector<Move>;

    inline auto getWhiteAttacks() const -> bitmask {
        return white_attacks;
    }
    inline auto getBlackAttacks() const -> bitmask {
        return black_attacks;
    }

    MoveGenerator(Precomputed precomputed) : precomputed(precomputed) {
        white_attacks = 0;
        black_attacks = 0;
    }
};