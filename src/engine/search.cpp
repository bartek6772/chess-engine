#include "search.hpp"
#include "evaluation.hpp"
#include "move_generator.hpp"
#include "move_list.hpp"
#include "pieces.hpp"
#include <chrono>
#include <thread>
#include <vector>

namespace Search {

namespace {
    constexpr int MATE = 99999;
    constexpr int INF = 100'000'000;
    constexpr int STOP_INTERVAL = 65536;

    int quiescence(Board& board, int alpha, int beta, SearchStats& stats) {
        stats.nodes++;
        stats.quiescence_nodes++;

        if (stats.stop_search) {
            return 0;
        }

        int stand_pat = Evaluation::evaluateRelative(board);
        if (stand_pat >= beta) return beta;
        if (alpha < stand_pat) alpha = stand_pat;

        MoveList captures = MoveGenerator::generateCaptures(board);

        for (const Move& move : captures) {
            board.makeMove(move);
            int score = -quiescence(board, -beta, -alpha, stats);
            board.unmakeMove();

            if (score >= beta) return beta;
            if (score > alpha) alpha = score;
        }

        return alpha;
    }

    int negamax(
        Board& board, int depth, int alpha, int beta, std::vector<Move>& pv, SearchStats& stats) {
        stats.nodes++;

        if (stats.stop_search) {
            return 0;
        }

        if (depth == 0) {
            pv.clear();
            // return Evaluation::evaluateRelative(board);
            return quiescence(board, alpha, beta, stats);
        }

        MoveList moves = MoveGenerator::generateLegalMoves(board);

        if (moves.size() == 0) {
            pv.clear();
            int color = board.white_to_move ? Pieces::White : Pieces::Black;
            if (MoveGenerator::isCheck(board, color)) {
                return -MATE;
            }
            return 0;
        }

        std::vector<Move> child_pv;
        child_pv.reserve(depth);

        for (const Move& move : moves) {
            board.makeMove(move);
            int eval = -negamax(board, depth - 1, -beta, -alpha, child_pv, stats);
            board.unmakeMove();

            if (eval >= beta) {
                stats.beta_cutoffs++;
                return beta;
            }

            if (eval > alpha) {
                alpha = eval;

                pv.clear();
                pv.push_back(move);
                pv.insert(pv.end(), child_pv.begin(), child_pv.end());
            }
        }

        return alpha;
    }
} // namespace

SearchResult findBestMove(Board& board, int depth, int time_ms) {

    SearchStats stats;
    std::vector<Move> best_pv;
    int best_score = 0;
    auto start = std::chrono::high_resolution_clock::now();

    std::thread timer([&stats, time_ms]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(time_ms));
        stats.stop_search = true;
    });

    for (int current_depth = 1; current_depth <= depth; current_depth++) {
        std::vector<Move> pv;
        int score = negamax(board, current_depth, -INF, INF, pv, stats);

        if (stats.stop_search) {
            break;
        }

        best_pv = pv;
        best_score = score;
        stats.depth = current_depth;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    double nps = 0;
    if (elapsed != 0) {
        nps = (double)stats.nodes / ((double)elapsed / 1000.0);
    }

    stats.time_ms = elapsed;
    stats.nodes_per_second = nps;
    int absolute_score = board.white_to_move ? best_score : -best_score;

    timer.detach();

    return {
        .best_move = best_pv.empty() ? Move() : best_pv[0],
        .score = absolute_score,
        .pv = best_pv,
        .stats = stats,
    };
}

} // namespace Search