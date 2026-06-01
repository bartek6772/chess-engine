#pragma once

#include "board.hpp"

namespace Evaluation {

constexpr int PawnValue = 100;
constexpr int KnightValue = 320;
constexpr int BishopValue = 350;
constexpr int RookValue = 500;
constexpr int QueenValue = 900;

int getPieceValue(Piece piece);
int evaluate(const Board& board);
int evaluateRelative(const Board& board);

} // namespace Evaluation