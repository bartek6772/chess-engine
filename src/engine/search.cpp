#include "search.hpp"
#include "evaluation.hpp"
#include "move_generator.hpp"
#include "move_list.hpp"
#include "pieces.hpp"
#include <chrono>
#include <vector>

namespace Search {

namespace {
    constexpr int MATE = 99999;
    constexpr int INF = 100'000'000;

    int quiescence(Board& board, int alpha, int beta, SearchStats& stats) {
        stats.nodes++;
        stats.quiescence_nodes++;

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

SearchResult findBestMove(Board& board, int depth) {

    SearchStats stats;
    std::vector<Move> pv;

    auto start = std::chrono::high_resolution_clock::now();
    int score = negamax(board, depth, -INF, INF, pv, stats);
    auto end = std::chrono::high_resolution_clock::now();

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    double nps = 0;
    if (elapsed != 0) {
        nps = (double)stats.nodes / ((double)elapsed / 1000.0);
    }

    stats.time_ms = elapsed;
    stats.nodes_per_second = nps;

    int absolute_score = -score;

    return {
        .best_move = pv.empty() ? Move() : pv[0],
        .score = absolute_score,
        .pv = pv,
        .stats = stats,
    };
}

} // namespace Search