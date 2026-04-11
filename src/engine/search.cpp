#include "search.hpp"
#include "board.hpp"
#include "evaluation.hpp"
#include "move_generator.hpp"
#include "move_list.hpp"
#include "pieces.hpp"
#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

namespace Search {

namespace {
    constexpr int MATE = 99999;
    constexpr int INF = 100'000'000;
    constexpr int STOP_INTERVAL = 1024;

    struct SearchContext {
        Board& board;
        SearchStats& stats;
        std::atomic<bool>& stop_search;
    };

    int quiescence(SearchContext& ctx, int alpha, int beta) {
        ctx.stats.nodes++;
        ctx.stats.quiescence_nodes++;

        if (ctx.stop_search) {
            return 0;
        }

        int stand_pat = Evaluation::evaluateRelative(ctx.board);
        if (stand_pat >= beta) return beta;
        if (alpha < stand_pat) alpha = stand_pat;

        MoveList captures = MoveGenerator::generateCaptures(ctx.board);

        for (const Move& move : captures) {
            ctx.board.makeMove(move);
            int score = -quiescence(ctx, -beta, -alpha);
            ctx.board.unmakeMove();

            if (score >= beta) return beta;
            if (score > alpha) alpha = score;
        }

        return alpha;
    }

    int negamax(SearchContext& ctx, int depth, int alpha, int beta, std::vector<Move>& pv) {
        ctx.stats.nodes++;

        if (ctx.stop_search) {
            return 0;
        }

        if (depth == 0) {
            pv.clear();
            // return Evaluation::evaluateRelative(board);
            return quiescence(ctx, alpha, beta);
        }

        MoveList moves = MoveGenerator::generateLegalMoves(ctx.board);

        if (moves.size() == 0) {
            pv.clear();
            int color = ctx.board.white_to_move ? Pieces::White : Pieces::Black;
            if (MoveGenerator::isCheck(ctx.board, color)) {
                return -MATE;
            }
            return 0;
        }

        std::vector<Move> child_pv;
        child_pv.reserve(depth);

        for (const Move& move : moves) {
            ctx.board.makeMove(move);
            int eval = -negamax(ctx, depth - 1, -beta, -alpha, child_pv);
            ctx.board.unmakeMove();

            if (eval >= beta) {
                ctx.stats.beta_cutoffs++;
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

SearchResult findBestMove(Board& board, int depth, int time_ms, std::atomic<bool>& external_stop) {

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<Move> best_pv;
    int best_score = 0;

    SearchStats stats;
    SearchContext ctx{ board, stats, external_stop };

    std::thread timer([&ctx, time_ms]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(time_ms));
        ctx.stop_search = true;
    });

    for (int current_depth = 1; current_depth <= depth; current_depth++) {
        std::vector<Move> pv;
        int score = negamax(ctx, current_depth, -INF, INF, pv);

        if (ctx.stop_search) {
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
    stats.mln_nodes_per_second = nps / 1'000'000;
    int absolute_score = board.white_to_move ? best_score : -best_score;

    timer.detach();

    return {
        .best_move = best_pv.empty() ? Move() : best_pv[0],
        .score = absolute_score,
        .pv = best_pv,
        .stats = stats,
    };
}

SearchResult findBestMove(Board& board, int depth, int time_ms) {
    std::atomic<bool> stop = false;
    return findBestMove(board, depth, time_ms, stop);
}
} // namespace Search