#pragma once

#include "constants.hpp"
#include "hashes.hpp"
#include "move.hpp"
#include "pieces.hpp"
#include <array>
#include <string>

constexpr int MAX_GAME_MOVES = 1024;

struct Board {
    std::array<Piece, BoardSize> squares{};
    std::array<bitmask, MaxPiecesCount> bitboards{};

    bitmask white_pieces{};
    bitmask black_pieces{};

    Piece::Color color_to_move = Piece::White;
    int enpassant_square = -1;
    int halfmove_clock = 0;

    bool whiteToMove() const {
        return color_to_move == Piece::White;
    }

    static constexpr int white_king_castle = 1;
    static constexpr int white_queen_castle = 2;
    static constexpr int black_king_castle = 4;
    static constexpr int black_queen_castle = 8;
    int castling_rights;

    void addPiece(int square, Piece piece);
    void removePiece(int square);
    void movePiece(int from, int to);

    auto loadFEN(const std::string& fen) -> bool;
    void makeMove(const Move& move);
    void unmakeMove();

    bool isRepetition() const;
    void clear();

    Board();

    struct HistoryState {
        Move move;
        Piece capture;

        // State before that move
        int halfmove_clock{};
        int enpassant_square{};
        int castling_rights{};
        unsigned long long hash{};
    };

    std::array<HistoryState, MAX_GAME_MOVES> history;
    int history_ptr = 0;

    inline static const Hashes hashes;
    unsigned long long hash;

    inline bool canCastle(int castle_type) const {
        return (castling_rights & castle_type) != 0;
    }

    inline void loadStartPos() {
        loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    }
};