#pragma once

#include "board.hpp"
#include "move_list.hpp"
#include <chrono>
#include <vector>

struct SearchStats {
    long long nodes = 0;
    long long quiescence_nodes = 0;
    long long beta_cutoffs = 0;
    long long nodes_per_second = 0;
    long long mln_nodes_per_second = 0;
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
    SearchResult findBestMove(int depth, int time);

    void stop();
    void enableInfo();

private:
    Board board;
    SearchStats stats;
    bool stop_search = false;
    bool info;

    Move killer_moves[64][2];
    std::chrono::time_point<std::chrono::steady_clock> start_point;
    int time_limit;

    int quiescence(int alpha, int beta);
    int negamax(int depth, int ply, int alpha, int beta, std::vector<Move>& pv);
    void scoreMoves(MoveList& moves, Move pv_move, int ply);
};