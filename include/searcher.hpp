#pragma once

#include "board.hpp"
#include "constants.hpp"
#include "move_list.hpp"
#include "transposition_table.hpp"
#include <array>
#include <atomic>
#include <chrono>
#include <cstring>
#include <vector>

struct SearchStats {
    unsigned long long nodes = 0;
    unsigned long long quiescence_nodes = 0;
    unsigned long long beta_cutoffs = 0;
    unsigned long long nodes_per_second = 0;
    unsigned long long mln_nodes_per_second = 0;
    long time_ms = 0;
    int depth = 0;
    int quienscence_depth = 0;
};

struct SearchResult {
    Move best_move;
    std::vector<Move> pv;
    SearchStats stats;
    int score;
};

class Searcher {
public:
    Searcher(Board board) : board(board){};
    SearchResult findBestMove(int depth, int time);

    void stop();
    void enableInfo();

    static void clearTable() {
        table.clear();
    }

    static int tableFillRate() {
        return table.fillRate();
    }

private:
    Board board;
    SearchStats stats;
    std::atomic<bool> stop_search = false;
    bool info = false;

    std::array<std::array<Move, 2>, MaxSearchDepth + 1> killer_moves{};
    std::chrono::time_point<std::chrono::steady_clock> start_point;
    int time_limit = 0;

    int quiescence(int alpha, int beta, int ply);
    int negamax(int depth, int ply, int alpha, int beta);
    void scoreMoves(MoveList& moves, Move pv_move, int ply);

    long searchTime();
    bool shouldStop();
    void reportInfo(int depth, int score, long time);

    static inline TranspositionTable table{ 100 };

    struct PVLine {
        int count = 0;
        std::array<Move, MaxSearchDepth + 1> moves;

        void push(const Move& move) {
            moves[count++] = move;
        }
        void clear() {
            count = 0;
        }

        void insert(const PVLine& other) {
            std::memcpy(&moves[1], other.moves.data(), other.count * sizeof(Move));
            count = other.count + 1;
        }
    };

    std::array<PVLine, MaxSearchDepth + 1> pv_table;
};