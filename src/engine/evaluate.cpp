#include "board.hpp"
#include "evaluation.hpp"
#include "pieces.hpp"
#include <bit>

namespace Evaluation {
namespace {

    int getPieceValue(int piece) {
        switch (Pieces::pieceType(piece)) {
            case Pieces::Pawn: return PawnValue;
            case Pieces::Knight: return KnightValue;
            case Pieces::Bishop: return BishopValue;
            case Pieces::Rook: return RookValue;
            case Pieces::Queen: return QueenValue;
        }
        return 0;
    }

} // namespace

int evaluate(const Board& board) {

    auto getPieceCount = [&](int piece) -> int {
        return std::popcount(board.bitboards[piece]);
    };

    int evaluation = 0;

    for (int piece : Pieces::white_pieces) {
        evaluation += getPieceCount(piece) * getPieceValue(piece);
    }

    for (int piece : Pieces::black_pieces) {
        evaluation -= getPieceCount(piece) * getPieceValue(piece);
    }

    return evaluation;
}

int evaluateRelative(const Board& board) {
    return evaluate(board) * (board.white_to_move ? 1 : -1);
}

} // namespace Evaluation