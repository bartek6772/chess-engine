#include "search.hpp"
#include "evaluation.hpp"
#include "move_generator.hpp"
#include "move_list.hpp"
#include "pieces.hpp"
#include <algorithm>
#include <vector>

namespace Search {

namespace {
    constexpr int MATE = 99999;
    constexpr int INF = 100'000'000;
} // namespace

int minimax(Board& board, int depth, int alpha, int beta, bool maximizing, std::vector<Move>& pv) {
    if (depth == 0) {
        pv.clear();
        return Evaluation::evaluate(board);
    }

    MoveList moves = MoveGenerator::generateLegalMoves(board);

    if (moves.size() == 0) {
        pv.clear();
        int color = board.white_to_move ? Pieces::White : Pieces::Black;
        if (MoveGenerator::isCheck(board, color)) {
            return board.white_to_move ? -MATE : MATE;
        } else {
            return 0;
        }
    }

    std::vector<Move> child_pv;
    if (maximizing) {

        int max_eval = -INF;
        for (const Move& move : moves) {
            board.makeMove(move);
            int eval = minimax(board, depth - 1, alpha, beta, false, child_pv);
            board.unmakeMove();

            if (eval > max_eval) {
                max_eval = eval;
                pv.clear();
                pv.push_back(move);
                pv.insert(pv.end(), child_pv.begin(), child_pv.end());
            }

            // max_eval = std::max(max_eval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break;
        }
        return max_eval;

    } else {

        int min_eval = INF;
        for (const Move& move : moves) {
            board.makeMove(move);
            int eval = minimax(board, depth - 1, alpha, beta, true, child_pv);
            board.unmakeMove();

            if (eval < min_eval) {
                min_eval = eval;
                pv.clear();
                pv.push_back(move);
                pv.insert(pv.end(), child_pv.begin(), child_pv.end());
            }

            // min_eval = std::min(min_eval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break;
        }
        return min_eval;
    }
}

SearchResult findBestMove(Board& board, int depth) {

    std::vector<Move> pv;
    int score = minimax(board, depth, -INF, INF, board.white_to_move, pv);

    return {
        .best_move = pv.empty() ? Move() : pv[0],
        .score = score,
        .pv = pv,
    };

    // Move best_move;
    // int best_value = board.white_to_move ? -INF : INF;

    // MoveList moves = MoveGenerator::generateLegalMoves(board);
    // for (const Move& move : moves) {
    //     board.makeMove(move);
    //     int eval = minimax(board, depth - 1, -INF, INF, !board.white_to_move);
    //     board.unmakeMove();

    //     if (board.white_to_move) {
    //         if (eval > best_value) {
    //             best_value = eval;
    //             best_move = move;
    //         }
    //     } else {
    //         if (eval < best_value) {
    //             best_value = eval;
    //             best_move = move;
    //         }
    //     }
    // }

    // return best_move;
}

} // namespace Search