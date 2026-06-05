#pragma once

#include "bitboard.hpp"
#include "constants.hpp"
#include "hashes.hpp"
#include "move.hpp"
#include "pieces.hpp"
#include "square.hpp"
#include <array>
#include <cstdint>
#include <string>

constexpr int MAX_GAME_MOVES = 1024;

struct Board {
    std::array<Piece, BoardSize> squares{};
    std::array<Bitboard, MaxPiecesCount> bitboards{};
    std::array<Bitboard, 2> pieces_by_color;

    Piece::Color color_to_move = Piece::White;
    Square enpassant_square = Squares::None;
    uint8_t halfmove_clock;
    uint8_t castling_rights;

    enum Castle : uint8_t {
        white_king_castle = 1,
        white_queen_castle = 2,
        black_king_castle = 4,
        black_queen_castle = 8,
    };

    struct HistoryState {
        uint64_t hash{};
        Move move;
        Square enpassant_square;
        Piece capture;
        uint8_t halfmove_clock{};
        uint8_t castling_rights{};
    };

    std::array<HistoryState, MAX_GAME_MOVES> history;
    int history_ptr = 0;

    inline static const Hashes hashes;
    uint64_t hash;

    void addPiece(Square square, Piece piece);
    void removePiece(Square square);
    void movePiece(Square from, Square to);

    auto loadFEN(const std::string& fen) -> bool;
    void makeMove(const Move& move);
    void unmakeMove();

    bool isRepetition() const;
    void clear();

    Board();

    bool whiteToMove() const {
        return color_to_move == Piece::White;
    }

    inline bool canCastle(Castle castle_type) const {
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

    inline Bitboard pieces(Piece::Color color) const {
        return pieces_by_color[color];
    }

    inline Bitboard pieces() const {
        return pieces_by_color[0] | pieces_by_color[1];
    }

    inline Bitboard pieces(Piece::Type type) const {
        return bitboards[Piece(type, Piece::White).value] |
               bitboards[Piece(type, Piece::Black).value];
    }
};