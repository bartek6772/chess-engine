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
#include <ostream>
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

Move partialSort(int index, MoveList& moves) {
    int best_index = index;
    for (int j = index + 1; j < moves.size(); j++) {
        if (moves[j].score() > moves[best_index].score()) {
            best_index = j;
        }
    }
    std::swap(moves[best_index], moves[index]);
    return moves[index];
}

int Searcher::quiescence(int alpha, int beta, int ply) {
    stats.quiescence_nodes++;
    stats.quienscence_depth = std::max(stats.quienscence_depth, ply);

    if (stats.nodes % STOP_INTERVAL == 0 && shouldStop()) {
        stop_search = true;
    }

    if (stop_search) {
        return 0;
    }

    Piece::Color color = board.color_to_move;
    bool is_check = MoveGenerator::isCheck(board, color);

    if (!is_check) {
        int stand_pat = Evaluation::evaluateRelative(board);
        if (stand_pat >= beta) return beta;
        if (alpha < stand_pat) alpha = stand_pat;
    }

    MoveList moves;
    if (ply < 2 && is_check) {
        moves = MoveGenerator::generateMoves(board);
    } else {
        moves = MoveGenerator::generateCaptures(board);
    }

    scoreMoves(moves, Move(), MaxSearchDepth);
    bool has_legal_move = false;

    for (int i = 0; i < moves.size(); i++) {
        if (stop_search) return 0;
        Move move = partialSort(i, moves);

        board.makeMove(move);
        if (MoveGenerator::isCheck(board, color)) {
            board.unmakeMove();
            continue;
        }
        has_legal_move = true;
        int score = -quiescence(-beta, -alpha, ply + 1);
        board.unmakeMove();

        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }

    if (is_check && !has_legal_move) {
        return -MATE + ply;
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
            } else if (entry->flag == LOWER_BOUND) {
                alpha = std::max(alpha, score);
            } else if (entry->flag == UPPER_BOUND) {
                beta = std::min(beta, score);
            }

            if (alpha >= beta) return score;
        }
    }

    if (ply == depth) {
        return quiescence(alpha, beta, 0);
    }

    MoveList moves = MoveGenerator::generateMoves(board);
    scoreMoves(moves, tt_move, ply);
    Piece::Color color = board.color_to_move;

    int best_eval = -INF;
    int alpha_original = alpha;

    for (int i = 0; i < moves.size(); i++) {
        if (stop_search) return 0;
        Move move = partialSort(i, moves);

        board.makeMove(move);
        if (MoveGenerator::isCheck(board, color)) {
            board.unmakeMove();
            continue;
        }
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

    if (best_eval == -INF) {
        if (MoveGenerator::isCheck(board, color)) {
            return -MATE + ply;
        }
        return 0;
    }

    NodeFlag flag = EXACT;
    if (best_eval <= alpha_original) flag = UPPER_BOUND;
    else if (best_eval >= beta) flag = LOWER_BOUND;

    table.store({
        board.hash,
        static_cast<int16_t>(scoreToTT(best_eval, ply)),
        static_cast<uint8_t>(remaining_depth),
        flag,
        (pv_table[ply].count == 0 ? Move() : pv_table[ply].moves[0]),
    });

    return best_eval;
}

void Searcher::scoreMoves(MoveList& moves, Move pv_move, int ply) {

    auto isCapture = [&](Move& move) -> bool {
        return board.squares[move.to()] != Pieces::None || move.type() == MoveType::EnPassant;
    };

    auto mvv_lva = [&](const Move& move) -> int {
        int from = move.from();
        int to = move.to();

        int attacker_value = Evaluation::getPieceValue(board.squares[from]);
        int victim_value = 0;

        if (move.type() == MoveType::EnPassant) {
            victim_value = Evaluation::PawnValue;
        } else {
            victim_value = Evaluation::getPieceValue(board.squares[to]);
        }

        return victim_value * 10 - attacker_value;
    };

    constexpr int PV_MOVE = 30'000;
    constexpr int QUEEN_PROMOTION = 29'000;
    constexpr int CAPTURE = 20'000;
    constexpr int MINOR_PROMOTION = 15'000;
    constexpr int KILLER_1 = 10'000;
    constexpr int KILLER_2 = 9'000;

    for (Move& move : moves) {
        if (move == pv_move) {
            move.setScore(PV_MOVE);
        } else if (isCapture(move)) {
            move.setScore(CAPTURE + mvv_lva(move));
        } else if (move == killer_moves[ply][0]) {
            move.setScore(KILLER_1);
        } else if (move == killer_moves[ply][1]) {
            move.setScore(KILLER_2);
        } else if (move.type() == MoveType::PromotionQueen) {
            move.setScore(QUEEN_PROMOTION);
        } else if (move.isPromotion()) {
            move.setScore(MINOR_PROMOTION);
        } else {
            move.setScore(0);
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
            if (initial_moves[i].score() > initial_moves[best_guess].score()) {
                best_guess = i;
            }
        }
        best_pv.push(initial_moves[best_guess]);
    }

    int best_score = 0;
    for (int current_depth = 1; current_depth <= depth; current_depth++) {

        uint64_t nodes_before = stats.quiescence_nodes;
        int score = negamax(current_depth, 0, -INF, INF);
        stats.nodes += stats.quiescence_nodes - nodes_before;

        if (stop_search) break;

        best_pv = pv_table[0];
        best_score = score;
        stats.depth = current_depth;

        long used = searchTime();
        int absolute_score = best_score * (board.whiteToMove() ? 1 : -1);
        reportInfo(current_depth, absolute_score, used);

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
    int absolute_score = best_score * (board.whiteToMove() ? 1 : -1);

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
    std::cout << "info"
    << " depth " << depth 
    << " seldepth " << stats.quienscence_depth 
    << " time " << time
    << " score cp " << score
    << " nodes " << stats.nodes
    << " nps " << nps
    << " hashfull " << table.fillRate()<< std::endl;
    // clang-format on
}