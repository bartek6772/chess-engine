#pragma once

#include "bitboard.hpp"
#include "constants.hpp"
#include "hashes.hpp"
#include "move.hpp"
#include "pieces.hpp"
#include "square.hpp"
#include <array>
#include <string>

constexpr int MAX_GAME_MOVES = 1024;

struct Board {
    std::array<Piece, BoardSize> squares{};
    std::array<Bitboard, MaxPiecesCount> bitboards{};

    Bitboard white_pieces;
    Bitboard black_pieces;

    Piece::Color color_to_move = Piece::White;
    Square enpassant_square = Squares::None;
    int halfmove_clock = 0;

    bool whiteToMove() const {
        return color_to_move == Piece::White;
    }

    static constexpr int white_king_castle = 1;
    static constexpr int white_queen_castle = 2;
    static constexpr int black_king_castle = 4;
    static constexpr int black_queen_castle = 8;
    int castling_rights;

    void addPiece(Square square, Piece piece);
    void removePiece(Square square);
    void movePiece(Square from, Square to);

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
        Square enpassant_square;
        int halfmove_clock{};
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

    inline Bitboard pieces(Piece::Type type, Piece::Color color) const {
        return bitboards[Piece(type, color).value];
    }

    inline Bitboard pieces(Piece piece) const {
        return bitboards[piece.value];
    }
};