#pragma once

#include "constants.hpp"
#include "move.hpp"
#include <array>
#include <string>
#include <vector>

struct Board {
    std::array<int, BoardSize> squares{};
    std::array<bitmask, MaxPiecesCount> bitboards{};

    bitmask white_pieces{};
    bitmask black_pieces{};

    bool white_to_move = true;
    int enpassant_square = -1;

    static constexpr int white_king_castle = 1;
    static constexpr int white_queen_castle = 2;
    static constexpr int black_king_castle = 4;
    static constexpr int black_queen_castle = 8;
    int castling_rights =
        white_king_castle | white_queen_castle | black_king_castle | black_queen_castle;

    void addPiece(int square, int piece);
    void removePiece(int square);
    void movePiece(int from, int to);

    auto loadFEN(const std::string& fen) -> bool;

    void makeMove(const Move& move);
    void unmakeMove();

    struct HistoryState {
        Move move;
        int capture;

        // State before that move
        int enpassant_square;
        int castling_rights;
    };

    Board();

    std::vector<HistoryState> history;
};