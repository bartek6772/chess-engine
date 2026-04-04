#pragma once

#include "board.hpp"
#include <vector>

namespace Search {

struct SearchResult {
    Move best_move;
    int score;
    std::vector<Move> pv;
};

int minimax(Board& board, int depth, int alpha, int beta, bool maximizing);
SearchResult findBestMove(Board& board, int depth);

} // namespace Search