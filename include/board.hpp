#pragma once

#include "constants.hpp"
#include "piece_list.hpp"
#include <array>
#include <string>

struct Board {
    std::array<int, BoardSize> squares{};
    std::array<bitmask, MaxPiecesCount> bitboards{};
    std::array<PieceList, MaxPiecesCount> pieceLists;

    bitmask white_pieces{};
    bitmask black_pieces{};

    bool white_to_move = true;

    void addPiece(int square, int piece);
    void removePiece(int square);

    auto loadFEN(const std::string& fen) -> bool;
};