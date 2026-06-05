#include "board.hpp"
#include "bitboard.hpp"
#include "constants.hpp"
#include "move.hpp"
#include "pieces.hpp"
#include "square.hpp"
#include <cassert>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

Board::Board() {
    clear();
}

void Board::clear() {
    bitboards.fill({});
    squares.fill(Piece());
    pieces_by_color.fill({});
    history_ptr = 0;
    enpassant_square = Squares::None;
    castling_rights = 0;
    halfmove_clock = 0;
    hash = 0;
}

void Board::addPiece(Square square, Piece piece) {
    squares[square] = piece;
    bitboards[piece.value] |= Bitboard(square);
    pieces_by_color[piece.color()] |= Bitboard(square);

    hash ^= hashes.piece_square[piece.value][square];
}

void Board::removePiece(Square square) {
    Piece piece = squares[square];

    squares[square] = Pieces::None;
    bitboards[piece.value] ^= Bitboard(square);
    pieces_by_color[piece.color()] ^= Bitboard(square);

    hash ^= hashes.piece_square[piece.value][square];
}

// does not handle captures
void Board::movePiece(Square from, Square to) {
    Piece piece = squares[from];

    squares[from] = Pieces::None;
    squares[to] = piece;

    bitboards[piece.value] ^= (Bitboard(from)) | (Bitboard(to));
    pieces_by_color[piece.color()] ^= (Bitboard(from)) | (Bitboard(to));

    hash ^= hashes.piece_square[piece.value][from];
    hash ^= hashes.piece_square[piece.value][to];
}

auto Board::loadFEN(const std::string& fen) -> bool {
    // TODO: handle errors and invalid strings, maybe return bool as veryfication

    clear();

    std::vector<std::string> parts;
    std::stringstream stream(fen);
    std::string str;

    while (std::getline(stream, str, ' ')) {
        parts.push_back(str);
    }

    if (parts.size() > 6) {
        return false;
    }

    { // 1. Position part
        if (parts.size() < 1) {
            return false;
        }

        int row = BoardLength - 1;
        int col = 0;

        for (char c : parts[0]) {
            if (c == '/') {
                row -= 1;
                col = 0;
            } else if (std::isdigit(c) != 0) {
                col += c - '0';
            } else {
                Piece piece = Piece::getPiece(c);
                addPiece(Square(col, row), piece);
                col++;
            }
        }
    }

    { // 2. Side to move
        if (parts.size() < 2) {
            return false;
        }

        if (parts[1] == "w") {
            color_to_move = Piece::White;
        } else if (parts[1] == "b") {
            color_to_move = Piece::Black;
        } else {
            return false;
        }
    }

    { // 3. Castling rights
        if (parts.size() < 3) {
            return false;
        }

        castling_rights = 0;
        for (char c : parts[2]) {
            switch (c) {
                case 'K': castling_rights |= white_king_castle; break;
                case 'k': castling_rights |= black_king_castle; break;
                case 'Q': castling_rights |= white_queen_castle; break;
                case 'q': castling_rights |= black_queen_castle; break;
            }
        }
    }

    { // 4. Enpassant
        if (parts.size() < 4) {
            return false;
        }

        if (parts[3] != "-") {
            int file = parts[3][0] - 'a';
            int rank = parts[3][1] - '1';
            enpassant_square = Square(file, rank);
        } else {
            enpassant_square = Squares::None;
        }
    }

    { // 5. 50 moves rule counter
        if (parts.size() < 5) {
            return false;
        }
        halfmove_clock = std::stoi(parts[4]);
    }

    { // 6. Total moves counter
        if (parts.size() < 6) {
            return false;
        }
        // fullmove_number = std::stoi(parts[5]);
    }

    hash ^= hashes.castling_rights[castling_rights];

    if (!whiteToMove()) {
        hash ^= hashes.side_to_move;
    }

    if (enpassant_square != Squares::None) {
        hash ^= hashes.enpassant_file[enpassant_square.file()];
    }

    return true;
}

void Board::makeMove(const Move& move) {
    assert(!move.isNull());
    assert(history_ptr < MAX_GAME_MOVES);

    Piece piece = squares[move.from()];
    Piece capture = squares[move.to()];

    HistoryState new_state{
        hash,
        move,
        enpassant_square,
        capture,
        halfmove_clock,
        castling_rights,
    };
    history[history_ptr++] = new_state;

    switch (move.from()) {
        case Squares::E1: castling_rights &= ~(white_king_castle | white_queen_castle); break;
        case Squares::E8: castling_rights &= ~(black_king_castle | black_queen_castle); break;
        case Squares::A1: castling_rights &= ~white_queen_castle; break;
        case Squares::H1: castling_rights &= ~white_king_castle; break;
        case Squares::A8: castling_rights &= ~black_queen_castle; break;
        case Squares::H8: castling_rights &= ~black_king_castle; break;
    }

    switch (move.to()) {
        case Squares::A1: castling_rights &= ~white_queen_castle; break;
        case Squares::H1: castling_rights &= ~white_king_castle; break;
        case Squares::A8: castling_rights &= ~black_queen_castle; break;
        case Squares::H8: castling_rights &= ~black_king_castle; break;
    }

    if (capture != Pieces::None) {
        removePiece(move.to());
    }

    if (move.isPromotion()) {
        removePiece(move.from());

        const Piece::Type type = [&]() {
            switch (move.type()) {
                case MoveType::PromotionRook: return Piece::Rook;
                case MoveType::PromotionKnight: return Piece::Knight;
                case MoveType::PromotionBishop: return Piece::Bishop;
                default: return Piece::Queen;
            }
        }();

        addPiece(move.to(), Piece(type, color_to_move));
        enpassant_square = Squares::None;
    } else {

        if (move.type() == MoveType::Castling) {
            Square rook_start;
            Square rook_target;
            int rank = move.to().rank();

            if (move.to() == Squares::G1 || move.to() == Squares::G8) {
                rook_start = Square(7, rank);
                rook_target = Square(5, rank);
            } else {
                rook_start = Square(0, rank);
                rook_target = Square(3, rank);
            }

            if (whiteToMove()) {
                castling_rights &= ~(white_queen_castle | white_king_castle);
            } else {
                castling_rights &= ~(black_queen_castle | black_king_castle);
            }

            movePiece(rook_start, rook_target);
        }

        if (move.type() == MoveType::EnPassant) {
            const Square enpassant = [&]() {
                if (whiteToMove()) {
                    return move.to() - BoardLength;
                } else {
                    return move.to() + BoardLength;
                }
            }();
            removePiece(enpassant);
        }

        if (move.type() == MoveType::DoublePush) {
            if (whiteToMove()) {
                enpassant_square = move.to() - BoardLength;
            } else {
                enpassant_square = move.to() + BoardLength;
            }
        } else {
            enpassant_square = Squares::None;
        }

        movePiece(move.from(), move.to());
    }

    color_to_move = Piece::flipColor(color_to_move);

    if (castling_rights != new_state.castling_rights) {
        hash ^= hashes.castling_rights[new_state.castling_rights];
        hash ^= hashes.castling_rights[castling_rights];
    }

    if (new_state.enpassant_square != Squares::None) {
        hash ^= hashes.enpassant_file[new_state.enpassant_square.file()];
    }

    if (enpassant_square != Squares::None) {
        hash ^= hashes.enpassant_file[enpassant_square.file()];
    }

    hash ^= hashes.side_to_move;

    if (piece.type() == Piece::Pawn || capture != Pieces::None) {
        halfmove_clock = 0;
    } else {
        halfmove_clock++;
    }
}

void Board::unmakeMove() {
    assert(history_ptr > 0);

    HistoryState state = history[--history_ptr];
    Move& move = state.move;

    color_to_move = Piece::flipColor(color_to_move);
    enpassant_square = state.enpassant_square;
    castling_rights = state.castling_rights;
    halfmove_clock = state.halfmove_clock;

    if (move.isPromotion()) {
        removePiece(move.to());
        addPiece(move.from(), Piece(Piece::Pawn, color_to_move));
    } else {

        if (move.type() == MoveType::Castling) {
            Square rook_start;
            Square rook_target;
            int rank = move.to().rank();

            if (move.to() == Squares::G1 || move.to() == Squares::G8) {
                rook_start = Square(7, rank);
                rook_target = Square(5, rank);
            } else {
                rook_start = Square(0, rank);
                rook_target = Square(3, rank);
            }

            movePiece(rook_target, rook_start);
        }

        if (move.type() == MoveType::EnPassant) {
            const Square enpassant = [&]() {
                if (whiteToMove()) {
                    return move.to() - BoardLength;
                } else {
                    return move.to() + BoardLength;
                }
            }();
            addPiece(enpassant, Piece(Piece::Pawn, Piece::flipColor(color_to_move)));
        }

        movePiece(move.to(), move.from());
    }

    if (state.capture != Pieces::None) {
        addPiece(move.to(), state.capture);
    }

    hash = state.hash;
}

bool Board::isRepetition() const {
    int start = std::max(0, history_ptr - halfmove_clock);

    for (int i = history_ptr - 2; i >= start; i -= 2) {
        if (history[i].hash == hash) {
            return true;
        }
    }
    return false;
}