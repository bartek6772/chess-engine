#pragma once

#include "board.hpp"
#include "constants.hpp"
#include "magics.hpp"
#include "move_list.hpp"
#include "precomputed.hpp"

// TODO: Precomputed can be embeded in MoveGenerator, removing the need to pass it in constructor

class MoveGenerator {
private:
    Precomputed precomputed;
    Magics magics;

    bitmask white_attacks;
    bitmask black_attacks;

    bool isSquareAttacked(const Board& board, int square, int attacker_color) const;

    void generateKnightMoves(const Board& board, MoveList& moves, int color) const;
    void generateRookMoves(const Board& board, MoveList& moves, int color) const;
    void generateBishopMoves(const Board& board, MoveList& moves, int color) const;
    void generateQueenMoves(const Board& board, MoveList& moves, int color) const;
    void generatePawnMoves(const Board& board, MoveList& moves, int color) const;
    void generateKingMoves(const Board& board, MoveList& moves, int color) const;

public:
    auto generateMoves(const Board& board) -> MoveList;
    auto generateLegalMoves(Board& board) -> MoveList;

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