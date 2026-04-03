#pragma once
#include "board.hpp"

namespace Diagnostics {

struct PerftResults {
    long long total;
    long long castles;
    long long captures;
    long long promotions;
    long long enpassants;
};

PerftResults runPerft(Board& board, int depth);

} // namespace Diagnostics