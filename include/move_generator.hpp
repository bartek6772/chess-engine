#pragma once

#include "board.hpp"
#include "move.hpp"
#include "precomputed.hpp"
#include <vector>

class MoveGenerator {
private:
    Precomputed precomputed;

    void generateKnightMoves(const Board& board, std::vector<Move>& moves);
    void generateRookMoves(const Board& board, std::vector<Move>& moves);
    void generateBishopMoves(const Board& board, std::vector<Move>& moves);
    void generateQueenMoves(const Board& board, std::vector<Move>& moves);

public:
    auto generateMoves(const Board& board) -> std::vector<Move>;

    MoveGenerator(Precomputed precomputed) : precomputed(precomputed) {}
};