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

    int depth = 0;
    bool stop_search = false;
};

struct SearchResult {
    Move best_move;
    int score;
    std::vector<Move> pv;

    SearchStats stats;
};

SearchResult findBestMove(Board& board, int depth, int time_ms);

} // namespace Search