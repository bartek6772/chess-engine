#pragma once

#include "board.hpp"
#include "move.hpp"
#include "precomputed.hpp"
#include <vector>

class MoveGenerator {
private:
    Precomputed precomputed;

    void generateKnightMoves(const Board& board, std::vector<Move>& moves) const;
    void generateRookMoves(const Board& board, std::vector<Move>& moves) const;
    void generateBishopMoves(const Board& board, std::vector<Move>& moves) const;
    void generateQueenMoves(const Board& board, std::vector<Move>& moves) const;
    void generatePawnMoves(const Board& board, std::vector<Move>& moves) const;

public:
    auto generateMoves(const Board& board) const -> std::vector<Move>;
    auto generateLegalMoves(Board& board) const -> std::vector<Move>;

    MoveGenerator(Precomputed precomputed) : precomputed(precomputed) {
    }
};