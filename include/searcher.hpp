#pragma once

#include "board.hpp"
#include "move_list.hpp"
#include "transposition_table.hpp"
#include <array>
#include <atomic>
#include <chrono>
#include <vector>

struct SearchStats {
    unsigned long long nodes = 0;
    unsigned long long quiescence_nodes = 0;
    unsigned long long beta_cutoffs = 0;
    unsigned long long nodes_per_second = 0;
    unsigned long long mln_nodes_per_second = 0;
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
    std::atomic<bool> stop_search = false;
    bool info;

    std::array<std::array<Move, 2>, 64> killer_moves{};
    std::chrono::time_point<std::chrono::steady_clock> start_point;
    int time_limit;

    int quiescence(int alpha, int beta);
    int negamax(int depth, int ply, int alpha, int beta, std::vector<Move>& pv);
    void scoreMoves(MoveList& moves, Move pv_move, int ply);

    long searchTime();
    bool shouldStop();

    static inline TranspositionTable table;
};