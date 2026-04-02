#pragma once

#include "constants.hpp"
#include "move.hpp"
#include <array>
#include <string>

static constexpr int MAX_GAME_MOVES = 1024;

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
    int castling_rights;

    void addPiece(int square, int piece);
    void removePiece(int square);
    void movePiece(int from, int to);

    auto loadFEN(const std::string& fen) -> bool;

    void makeMove(const Move& move);
    void unmakeMove();

    inline bool canCastle(int castle_type) const {
        return (castling_rights & castle_type) != 0;
    }

    struct HistoryState {
        Move move;
        int capture{};

        // State before that move
        int enpassant_square{};
        int castling_rights{};
    };

    Board();

    std::array<HistoryState, MAX_GAME_MOVES> history;
    int history_ptr = 0;
};