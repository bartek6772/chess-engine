#include "search.hpp"
#include "evaluation.hpp"
#include "move_generator.hpp"
#include "move_list.hpp"
#include "pieces.hpp"
#include <algorithm>

namespace Search {

namespace {
    constexpr int MATE = 99999;
    constexpr int INF = 10000000;
} // namespace

int minimax(Board& board, int depth, int alpha, int beta, bool maximizing) {
    if (depth == 0) return Evaluation::evaluate(board);

    MoveList moves = MoveGenerator::generateLegalMoves(board);
    int color = board.white_to_move ? Pieces::White : Pieces::Black;

    if (moves.size() == 0) {
        if (MoveGenerator::isCheck(board, color)) {
            return board.white_to_move ? -MATE : MATE;
        } else {
            return 0;
        }
    }

    if (maximizing) {

        int max_eval = -INF;
        for (const Move& move : moves) {
            board.makeMove(move);
            int eval = minimax(board, depth - 1, alpha, beta, false);
            board.unmakeMove();

            max_eval = std::max(max_eval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break;
        }
        return max_eval;

    } else {

        int min_eval = INF;
        for (const Move& move : moves) {
            board.makeMove(move);
            int eval = minimax(board, depth - 1, alpha, beta, true);
            board.unmakeMove();

            min_eval = std::min(min_eval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break;
        }
        return min_eval;
    }
}

Move findBestMove(Board& board, int depth) {
    Move best_move;
    int best_value = board.white_to_move ? -INF : INF;

    MoveList moves = MoveGenerator::generateLegalMoves(board);
    for (const Move& move : moves) {
        board.makeMove(move);
        int eval = minimax(board, depth - 1, -INF, INF, !board.white_to_move);
        board.unmakeMove();

        if (board.white_to_move) {
            if (eval > best_value) {
                best_value = eval;
                best_move = move;
            }
        } else {
            if (eval < best_value) {
                best_value = eval;
                best_move = move;
            }
        }
    }

    return best_move;
}

} // namespace Search