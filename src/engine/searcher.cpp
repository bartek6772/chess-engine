#include "searcher.hpp"
#include "board.hpp"
#include "constants.hpp"
#include "evaluation.hpp"
#include "move.hpp"
#include "move_generator.hpp"
#include "move_list.hpp"
#include "pieces.hpp"
#include "transposition_table.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <utility>
#include <vector>

constexpr int MATE = 99999;
constexpr int INF = 100'000'000;
constexpr int STOP_INTERVAL = 1024;

int scoreToTT(int score, int ply) {
    if (score > 90000) return score + ply;
    if (score < -90000) return score - ply;
    return score;
}

int scoreFromTT(int score, int ply) {
    if (score > 90000) return score - ply;
    if (score < -90000) return score + ply;
    return score;
}

long Searcher::searchTime() {
    using namespace std::chrono;
    auto now = steady_clock::now();
    auto elapsed = duration_cast<milliseconds>(now - start_point).count();
    return elapsed;
}

bool Searcher::shouldStop() {
    return searchTime() >= time_limit;
}

int Searcher::quiescence(int alpha, int beta) {
    stats.nodes++;
    stats.quiescence_nodes++;

    if (stats.nodes % STOP_INTERVAL == 0 && shouldStop()) {
        stop_search = true;
    }

    if (stop_search) {
        return 0;
    }

    int stand_pat = Evaluation::evaluateRelative(board);
    if (stand_pat >= beta) return beta;
    if (alpha < stand_pat) alpha = stand_pat;

    MoveList captures = MoveGenerator::generateCaptures(board);

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

int Searcher::negamax(int depth, int ply, int alpha, int beta) {
    stats.nodes++;

    if (stats.nodes % STOP_INTERVAL == 0 && shouldStop()) {
        stop_search = true;
    }

    if (stop_search) {
        return 0;
    }

    if (board.halfmove_clock >= 100 || (ply > 0 && board.isRepetition())) {
        return 0;
    }

    pv_table[ply].clear();

    int remaining_depth = depth - ply;
    Move tt_move = Move();
    const TTEntry* entry = table.get(board.hash);
    if (entry) {
        tt_move = entry->best_move;
        if (entry->depth >= remaining_depth) {
            int score = scoreFromTT(entry->score, ply);

            if (entry->flag == EXACT) {
                pv_table[ply].push(entry->best_move);
                return score;
            } else if (entry->flag == LOWER_BOUND) alpha = std::max(alpha, score);
            else if (entry->flag == UPPER_BOUND) beta = std::min(beta, score);

            if (alpha >= beta) return score;
        }
    }

    if (ply == depth) {
        return quiescence(alpha, beta);
    }

    MoveList moves = MoveGenerator::generateLegalMoves(board);
    scoreMoves(moves, tt_move, ply);

    if (moves.size() == 0) {
        int color = board.white_to_move ? Pieces::White : Pieces::Black;
        if (MoveGenerator::isCheck(board, color)) {
            return -MATE + ply;
        }
        return 0;
    }

    int best_eval = -INF;
    int alpha_original = alpha;

    for (int i = 0; i < moves.size(); i++) {
        if (stop_search) return 0;

        int best_index = i;
        for (int j = i + 1; j < moves.size(); j++) {
            if (moves[j].score > moves[best_index].score) {
                best_index = j;
            }
        }
        std::swap(moves[best_index], moves[i]);
        Move& move = moves[i];

        board.makeMove(move);
        int eval = -negamax(depth, ply + 1, -beta, -alpha);
        board.unmakeMove();

        if (eval > best_eval) {
            best_eval = eval;

            if (eval > alpha) {
                alpha = eval;

                pv_table[ply].clear();
                pv_table[ply].push(move);
                pv_table[ply].insert(pv_table[ply + 1]);
            }
        }

        if (eval >= beta) {
            if (board.squares[move.to()] == Pieces::None) {
                killer_moves[ply][1] = killer_moves[ply][0];
                killer_moves[ply][0] = move;
            }

            stats.beta_cutoffs++;
            break;
        }
    }

    if (stop_search) {
        return 0; // prevents overwriting good TT entry
    }

    NodeFlag flag = EXACT;
    if (best_eval <= alpha_original) flag = UPPER_BOUND;
    else if (best_eval >= beta) flag = LOWER_BOUND;

    table.store({
        board.hash,
        static_cast<int16_t>(remaining_depth),
        static_cast<int16_t>(scoreToTT(best_eval, ply)),
        flag,
        (pv_table[ply].count == 0 ? Move() : pv_table[ply].moves[0]),
    });

    return best_eval;
}

void Searcher::scoreMoves(MoveList& moves, Move pv_move, int ply) {

    auto isCapture = [&](Move& move) -> bool {
        return board.squares[move.to()] != Pieces::None;
    };

    auto mvv_lva = [&](int from, int to) -> int {
        int attacker_value = Evaluation::getPieceValue(board.squares[from]);
        int victim_value = Evaluation::getPieceValue(board.squares[to]);
        return victim_value * 10 - attacker_value;
    };

    constexpr int LAST_MOVE = 30'000;
    constexpr int CAPTURE = 20'000;
    constexpr int KILLER_1 = 10'000;
    constexpr int KILLER_2 = 9'000;

    for (Move& move : moves) {
        if (move == pv_move) {
            move.score = LAST_MOVE;
        } else if (isCapture(move)) {
            move.score = CAPTURE + mvv_lva(move.from(), move.to());
        } else if (move == killer_moves[ply][0]) {
            move.score = KILLER_1;
        } else if (move == killer_moves[ply][1]) {
            move.score = KILLER_2;
        } else {
            move.score = 0;
        }
    }
}

SearchResult Searcher::findBestMove(int depth, int time) {
    start_point = std::chrono::steady_clock::now();
    stats = SearchStats();
    stop_search = false;
    time_limit = time;

    PVLine best_pv;

    MoveList initial_moves = MoveGenerator::generateLegalMoves(board);
    if (initial_moves.size() > 0) {
        scoreMoves(initial_moves, Move(), 0);

        int best_guess = 0;
        for (int i = 0; i < initial_moves.size(); i++) {
            if (initial_moves[i].score > initial_moves[best_guess].score) {
                best_guess = i;
            }
        }
        std::swap(initial_moves[best_guess], initial_moves[0]);
    }

    int best_score = 0;
    for (int current_depth = 1; current_depth <= depth; current_depth++) {
        int score = negamax(current_depth, 0, -INF, INF);

        if (stop_search) {
            break;
        }

        best_pv = pv_table[0];
        best_score = score;
        stats.depth = current_depth;

        long used = searchTime();
        reportInfo(current_depth, score, used);

        if (time_limit - used <= used * 4) {
            break;
        }
    }

    stats.time_ms = searchTime();
    if (stats.time_ms > 0) {
        unsigned long long nps = (stats.nodes * 1000) / stats.time_ms;
        stats.nodes_per_second = nps;
        stats.mln_nodes_per_second = nps / 1000000;
    }
    int absolute_score = board.white_to_move ? best_score : -best_score;

    std::vector<Move> pv(best_pv.moves.begin(), best_pv.moves.begin() + best_pv.count);

    return {
        .best_move = pv.empty() ? initial_moves[0] : best_pv.moves[0],
        .pv = pv,
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

void Searcher::reportInfo(int depth, int score, long time) {
    if (!info) return;

    unsigned long long nps = stats.nodes;
    if (time != 0) {
        nps = (stats.nodes * 1000) / time;
    }

    // clang-format off
    std::cout << "info depth " << depth 
    << " time " << time 
    << " score cp " << score
    << " nodes " << stats.nodes
    << " nps " << nps << std::endl;
    // clang-format on
}