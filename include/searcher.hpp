#pragma once

#include "board.hpp"
#include "move_list.hpp"
#include <atomic>
#include <vector>

struct SearchStats {
    unsigned long long nodes = 0;
    unsigned long long quiescence_nodes = 0;
    unsigned long long beta_cutoffs = 0;
    double nodes_per_second = 0;
    double mln_nodes_per_second = 0;
    long time_ms = 0;
    int depth = 0;
};

struct SearchResult {
    Move best_move;
    std::vector<Move> pv;
    SearchStats stats;
    int score;
};

class Searcher {
public:
    Searcher(Board board);
    SearchResult findBestMove(int depth, int time_ms);

    void stop();
    void enableInfo();

private:
    Board board;
    std::atomic<bool> stop_search = false;
    SearchStats stats;
    bool info;

    Move killer_moves[64][2];

    int quiescence(int alpha, int beta);
    int negamax(int depth, int alpha, int beta, std::vector<Move>& pv);
    void scoreMoves(MoveList& moves, Move pv_move, int ply);
};