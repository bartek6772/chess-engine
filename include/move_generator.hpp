#pragma once

#include "bitboard.hpp"
#include "board.hpp"
#include "move_list.hpp"
#include "pieces.hpp"

namespace MoveGenerator {

auto generateMoves(const Board& board) -> MoveList;
auto generateLegalMoves(Board& board) -> MoveList;

auto generateCaptures(Board& board) -> MoveList;
auto generateLegalCaptures(Board& board) -> MoveList;

bool isCheck(const Board& board, Piece::Color color);

Bitboard getAttackers(Square square, const Board& board);
Bitboard getXrayAttackers(
    Square square, Square cleared_attacker, Bitboard occupied, const Board& board);

} // namespace MoveGenerator