#pragma once

#include "piece_list.hpp"
#include <array>

struct Board {
    std::array<int, BoardSize> squares{};
    std::array<PieceList, MaxPiecesCount> pieceLists;

    bool whiteToMove = true;

    void addPiece(int square, int piece);
    void removePiece(int square);
};