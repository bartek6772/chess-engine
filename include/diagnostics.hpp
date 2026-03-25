#pragma once
#include "board.hpp"
#include "move_generator.hpp"

namespace Diagnostics {

long long runPerft(Board& board, MoveGenerator& move_gen, int depth);

}