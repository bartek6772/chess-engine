#pragma once

#include "board.hpp"
#include "move_list.hpp"
#include "pieces.hpp"

namespace MoveGenerator {

auto generateMoves(const Board& board) -> MoveList;
auto generateLegalMoves(Board& board) -> MoveList;

auto generateCaptures(Board& board) -> MoveList;
auto generateLegalCaptures(Board& board) -> MoveList;

bool isCheck(const Board& board, Piece::Color color);

} // namespace MoveGenerator