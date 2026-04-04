#pragma once

#include "board.hpp"
#include <vector>

namespace Search {

struct SearchStats {
    unsigned long long nodes = 0;
    unsigned long long quiescence_nodes = 0;
    long time_ms = 0;
    double nodes_per_second = 0;
    unsigned long long beta_cutoffs = 0;
};

struct SearchResult {
    Move best_move;
    int score;
    std::vector<Move> pv;

    SearchStats stats;
};

SearchResult findBestMove(Board& board, int depth);

} // namespace Search