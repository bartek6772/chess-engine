#include "board.hpp"
#include "constants.hpp"
#include "evaluation.hpp"
#include "pieces.hpp"
#include <array>
#include <bit>

namespace Evaluation {
namespace {

    // clang-format off
    const std::array<int, 64> pawnPST = {
        0,  0,  0,  0,  0,  0,  0,  0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
        5,  5, 10, 25, 25, 10,  5,  5,
        0,  0,  0, 20, 20,  0,  0,  0,
        5, -5,-10,  0,  0,-10, -5,  5,
        5, 10, 10,-20,-20, 10, 10,  5,
        0,  0,  0,  0,  0,  0,  0,  0
    };

    const std::array<int, 64> knightPST = {
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50
    };

    const std::array<int, 64> bishopPST = {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20
    };

    const std::array<int, 64> kingPST = {
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -10,-20,-20,-20,-20,-20,-20,-10,
        20, 20,  0,  0,  0,  0, 20, 20,
        20, 30, 10,  0,  0, 10, 30, 20
    };
    // clang-format on

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

    auto getPositionValue = [&](int piece, const std::array<int, BoardSize>& pst, int flip) -> int {
        int eval = 0;
        bitmask pieces = board.bitboards[piece];
        while (pieces) {
            int square = std::countr_zero(pieces);
            pieces &= (pieces - 1);

            eval += pst[square ^ flip];
        }
        return eval;
    };

    int evaluation = 0;

    constexpr int flip_rank = 56;
    evaluation += getPositionValue(Pieces::WhitePawn, pawnPST, flip_rank);
    evaluation += getPositionValue(Pieces::WhiteBishop, bishopPST, flip_rank);
    evaluation += getPositionValue(Pieces::WhiteKnight, knightPST, flip_rank);
    evaluation += getPositionValue(Pieces::WhiteKing, kingPST, flip_rank);

    evaluation -= getPositionValue(Pieces::BlackPawn, pawnPST, 0);
    evaluation -= getPositionValue(Pieces::BlackBishop, bishopPST, 0);
    evaluation -= getPositionValue(Pieces::BlackKnight, knightPST, 0);
    evaluation -= getPositionValue(Pieces::BlackKing, kingPST, 0);

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