#include "diagnostics.hpp"
#include "move.hpp"
#include "move_generator.hpp"
#include "move_list.hpp"

long long Diagnostics::runPerft(Board& board, MoveGenerator& move_gen, int depth) {

    MoveList moves = move_gen.generateLegalMoves(board);
    if (depth == 1) {
        return (long long)moves.size();
    }

    long long result = 0;
    for (Move& move : moves) {
        board.makeMove(move);
        result += runPerft(board, move_gen, depth - 1);
        board.unmakeMove();
    }

    return result;
}