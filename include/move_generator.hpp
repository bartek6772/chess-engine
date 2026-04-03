#pragma once

#include "board.hpp"
#include "move_list.hpp"

namespace MoveGenerator {

auto generateMoves(const Board& board) -> MoveList;
auto generateLegalMoves(Board& board) -> MoveList;

}; // namespace MoveGenerator