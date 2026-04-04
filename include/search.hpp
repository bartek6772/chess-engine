#pragma once

#include "board.hpp"

namespace Search {

int minimax(Board& board, int depth, int alpha, int beta, bool maximizing);
Move findBestMove(Board& board, int depth);

} // namespace Search