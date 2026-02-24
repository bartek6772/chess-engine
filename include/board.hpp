#pragma once

#include "constants.hpp"
#include "move.hpp"
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
    int enpassant_square = -1;

    void addPiece(int square, int piece);
    void removePiece(int square);
    void movePiece(int from, int to);

    auto loadFEN(const std::string& fen) -> bool;

    void makeMove(Move& move);
};