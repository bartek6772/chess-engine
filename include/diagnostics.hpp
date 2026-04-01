#pragma once
#include "board.hpp"
#include "move_generator.hpp"

namespace Diagnostics {

struct PerftResults {
    long long total;
    long long castles;
    long long captures;
    long long promotions;
    long long enpassants;
};

PerftResults runPerft(Board& board, MoveGenerator& move_gen, int depth);

} // namespace Diagnostics