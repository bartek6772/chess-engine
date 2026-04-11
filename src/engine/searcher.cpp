#include "searcher.hpp"
#include "board.hpp"
#include "evaluation.hpp"
#include "move_generator.hpp"
#include "move_list.hpp"
#include "pieces.hpp"
#include <atomic>
#include <chrono>
#include <memory>
#include <thread>
#include <vector>

constexpr int MATE = 99999;
constexpr int INF = 100'000'000;
constexpr int STOP_INTERVAL = 1024;

int Searcher::quiescence(int alpha, int beta) {
    stats.nodes++;
    stats.quiescence_nodes++;

    if (stop_search) {
        return 0;
    }

    int stand_pat = Evaluation::evaluateRelative(board);
    if (stand_pat >= beta) return beta;
    if (alpha < stand_pat) alpha = stand_pat;

    MoveList captures = MoveGenerator::generateCaptures(board);

    for (const Move& move : captures) {
        board.makeMove(move);
        int score = -quiescence(-beta, -alpha);
        board.unmakeMove();

        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }

    return alpha;
}

int Searcher::negamax(int depth, int alpha, int beta, std::vector<Move>& pv) {
    stats.nodes++;

    if (stop_search) {
        return 0;
    }

    if (depth == 0) {
        pv.clear();
        // return Evaluation::evaluateRelative(board);
        return quiescence(alpha, beta);
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
        int eval = -negamax(depth - 1, -beta, -alpha, child_pv);
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

SearchResult Searcher::findBestMove(int depth, int time_ms) {

    stats = SearchStats();
    stop_search = false;

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<Move> best_pv;
    int best_score = 0;

    auto timer_should_exit = std::make_shared<std::atomic<bool>>(false);
    std::thread timer([this, time_ms, timer_should_exit]() {
        auto start = std::chrono::high_resolution_clock::now();
        while (!*timer_should_exit) {
            auto now = std::chrono::high_resolution_clock::now();
            auto elapsed =
                std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();

            if (elapsed >= time_ms) {
                stop_search = true;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });

    for (int current_depth = 1; current_depth <= depth; current_depth++) {
        std::vector<Move> pv;
        int score = negamax(current_depth, -INF, INF, pv);

        if (stop_search) {
            break;
        }

        best_pv = pv;
        best_score = score;
        stats.depth = current_depth;
    }

    *timer_should_exit = true;
    if (timer.joinable()) timer.join();

    auto end = std::chrono::high_resolution_clock::now();
    stats.time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    if (stats.time_ms > 0) {
        double nps = (double)stats.nodes / ((double)stats.time_ms / 1000.0);
        stats.nodes_per_second = nps;
        stats.mln_nodes_per_second = nps / 1'000'000;
    }

    int absolute_score = board.white_to_move ? best_score : -best_score;

    return {
        .best_move = best_pv.empty() ? Move() : best_pv[0],
        .score = absolute_score,
        .pv = best_pv,
        .stats = stats,
    };
}

void Searcher::stop() {
    stop_search = true;
}

Searcher::Searcher(Board& board) : board(board) {
}