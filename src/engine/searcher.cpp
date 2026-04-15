#include "searcher.hpp"
#include "board.hpp"
#include "evaluation.hpp"
#include "move.hpp"
#include "move_generator.hpp"
#include "move_list.hpp"
#include "pieces.hpp"
#include <chrono>
#include <iostream>
#include <utility>
#include <vector>

constexpr int MATE = 99999;
constexpr int INF = 100'000'000;
constexpr int STOP_INTERVAL = 1024;

int Searcher::quiescence(int alpha, int beta) {
    stats.nodes++;
    stats.quiescence_nodes++;

    if (stats.nodes % STOP_INTERVAL == 0) {
        using namespace std::chrono;
        auto now = steady_clock::now();
        auto elapsed = duration_cast<milliseconds>(now - start_point).count();

        if (elapsed >= time_limit) {
            stop_search = true;
        }
    }

    if (stop_search) {
        return 0;
    }

    int stand_pat = Evaluation::evaluateRelative(board);
    if (stand_pat >= beta) return beta;
    if (alpha < stand_pat) alpha = stand_pat;

    MoveList captures = MoveGenerator::generateCaptures(board);
    // scoreMoves(captures, Move(), 0);

    for (const Move& move : captures) {
        if (stop_search) return 0;

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

    if (stats.nodes % STOP_INTERVAL == 0) {
        using namespace std::chrono;
        auto now = steady_clock::now();
        auto elapsed = duration_cast<milliseconds>(now - start_point).count();

        if (elapsed >= time_limit) {
            stop_search = true;
        }
    }

    if (stop_search) {
        return 0;
    }

    if (depth == 0) {
        pv.clear();
        // return Evaluation::evaluateRelative(board);
        return quiescence(alpha, beta);
    }

    MoveList moves = MoveGenerator::generateLegalMoves(board);
    scoreMoves(moves, pv.size() > 0 ? pv[0] : Move(), depth);

    if (moves.size() == 0) {
        pv.clear();
        int color = board.white_to_move ? Pieces::White : Pieces::Black;
        if (MoveGenerator::isCheck(board, color)) {
            return -MATE + depth;
        }
        return 0;
    }

    std::vector<Move> child_pv;
    child_pv.reserve(depth);

    for (int i = 0; i < moves.size(); i++) {

        if (stop_search) return 0;

        for (int j = i + 1; j < moves.size(); j++) {
            if (moves[j].score > moves[i].score) {
                std::swap(moves[j], moves[i]);
            }
        }
        Move& move = moves[i];

        board.makeMove(move);
        int eval = -negamax(depth - 1, -beta, -alpha, child_pv);
        board.unmakeMove();

        if (eval >= beta) {
            if (board.squares[move.to] == Pieces::None) {
                killer_moves[depth][1] = killer_moves[depth][0];
                killer_moves[depth][0] = move;
            }

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

void Searcher::scoreMoves(MoveList& moves, Move pv_move, int ply) {

    auto isCapture = [&](Move& move) -> bool {
        return board.squares[move.to] != Pieces::None;
    };

    auto mvv_lva = [&](int from, int to) -> int {
        int attacker_value = Evaluation::getPieceValue(board.squares[from]);
        int victim_value = Evaluation::getPieceValue(board.squares[to]);
        return victim_value * 10 - attacker_value;
    };

    constexpr int LAST_MOVE = 1000000;
    constexpr int CAPTURE = 100000;
    constexpr int KILLER_1 = 90000;
    constexpr int KILLER_2 = 80000;

    for (Move& move : moves) {
        if (move == pv_move) {
            move.score = LAST_MOVE;
        } else if (isCapture(move)) {
            move.score = CAPTURE + mvv_lva(move.from, move.to);
        } else if (move == killer_moves[ply][0]) {
            move.score = KILLER_1;
        } else if (move == killer_moves[ply][1]) {
            move.score = KILLER_2;
        } else {
            // position gain from PST
        }
    }
}

SearchResult Searcher::findBestMove(int depth, int time) {
    using namespace std::chrono;

    std::vector<Move> best_pv;
    stats = SearchStats();
    stop_search = false;
    int best_score = 0;

    time_limit = time;
    start_point = steady_clock::now();

    for (int current_depth = 1; current_depth <= depth; current_depth++) {
        std::vector<Move> pv;
        int score = negamax(current_depth, -INF, INF, pv);

        if (stop_search) {
            break;
        }

        best_pv = pv;
        best_score = score;
        stats.depth = current_depth;

        auto now = steady_clock::now();

        // Consult this, displaying total search time instead of single search
        long used = duration_cast<milliseconds>(now - start_point).count();
        // used = std::max(0L, used);

        if (time_limit - used <= used * 4) {
            break;
        }

        if (info) {
            // clang-format off
            std::cout << "info depth " << current_depth 
                << " time " << used 
                << " score cp " << score
                << " nodes " << stats.nodes << std::endl;
            // clang-format on
        }
    }

    auto end = steady_clock::now();
    stats.time_ms = duration_cast<milliseconds>(end - start_point).count();
    // stats.time_ms = std::max(0L, stats.time_ms);

    if (stats.time_ms > 0) {
        double nps = (double)stats.nodes / ((double)stats.time_ms / 1000.0);
        stats.nodes_per_second = nps;
        stats.mln_nodes_per_second = nps / 1'000'000;
    }
    int absolute_score = board.white_to_move ? best_score : -best_score;

    return {
        .best_move = best_pv.empty() ? Move() : best_pv[0],
        .pv = best_pv,
        .stats = stats,
        .score = absolute_score,
    };
}

void Searcher::stop() {
    stop_search = true;
}

void Searcher::enableInfo() {
    info = true;
}

Searcher::Searcher(Board board) : board(board) {
}