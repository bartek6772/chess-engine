#include "diagnostics.hpp"
#include "move.hpp"
#include "move_generator.hpp"
#include "move_list.hpp"
#include "pieces.hpp"

void perft(Board& board, Diagnostics::PerftResults& results, int depth) {

    auto isCapture = [&](const Move& move) {
        return board.squares[move.to] != Pieces::None || move.type == MoveType::EnPassant;
    };

    auto isCastle = [&](const Move& move) {
        return move.type == MoveType::Castling;
    };

    auto isEp = [&](const Move& move) {
        return move.type == MoveType::EnPassant;
    };

    auto isPromotion = [&](const Move& move) {
        return move.isPromotion();
    };

    MoveList moves = MoveGenerator::generateLegalMoves(board);
    if (depth == 1) {

        for (Move& move : moves) {
            if (isPromotion(move)) results.promotions++;
            if (isCapture(move)) results.captures++;
            if (isCastle(move)) results.castles++;
            if (isEp(move)) results.enpassants++;
            results.total++;
        }

        return;
    }

    for (Move& move : moves) {
        board.makeMove(move);
        perft(board, results, depth - 1);
        board.unmakeMove();
    }
}

Diagnostics::PerftResults Diagnostics::runPerft(Board& board, int depth) {
    PerftResults results{};
    perft(board, results, depth);
    return results;
}