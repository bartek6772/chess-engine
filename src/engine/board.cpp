#include "board.hpp"
#include "constants.hpp"
#include "move.hpp"
#include "pieces.hpp"
#include "utility.hpp"
#include <cassert>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

Board::Board() {
    clear();
}

void Board::clear() {
    bitboards.fill(0);
    squares.fill(Piece());
    white_pieces = 0;
    black_pieces = 0;
    history_ptr = 0;
    enpassant_square = -1;
    castling_rights = 0;
    halfmove_clock = 0;
    hash = 0;
}

void Board::addPiece(int square, Piece piece) {
    squares[square] = piece;
    bitboards[piece.value] |= setBit(square);

    if (piece.isWhite()) {
        white_pieces |= setBit(square);
    } else {
        black_pieces |= setBit(square);
    }

    hash ^= hashes.piece_square[piece.value][square];
}

void Board::removePiece(int square) {
    Piece piece = squares[square];
    squares[square] = Pieces::None;
    bitboards[piece.value] ^= setBit(square);

    if (piece.isWhite()) {
        white_pieces ^= setBit(square);
    } else {
        black_pieces ^= setBit(square);
    }

    hash ^= hashes.piece_square[piece.value][square];
}

// does not handle captures
void Board::movePiece(int from, int to) {
    Piece piece = squares[from];

    squares[from] = Pieces::None;
    squares[to] = piece;

    bitboards[piece.value] ^= (setBit(from)) | (setBit(to));

    if (piece.isWhite()) {
        white_pieces ^= (setBit(from)) | (setBit(to));
    } else {
        black_pieces ^= (setBit(from)) | (setBit(to));
    }

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
                addPiece(makeSquare(col, row), piece);
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
            enpassant_square = makeSquare(file, rank);
        } else {
            enpassant_square = -1;
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

    if (enpassant_square != -1) {
        hash ^= hashes.enpassant_file[fileOf(enpassant_square)];
    }

    return true;
}

void Board::makeMove(const Move& move) {

    assert(!move.isNull());

    Piece piece = squares[move.from()];
    Piece capture = squares[move.to()];

    HistoryState new_state{
        move,
        capture,
        halfmove_clock,
        enpassant_square,
        castling_rights,
        hash,
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
        if (move.type() == MoveType::PromotionQueen) {
            addPiece(move.to(), Piece{ Piece::Queen, color_to_move });
        } else if (move.type() == MoveType::PromotionBishop) {
            addPiece(move.to(), Piece{ Piece::Bishop, color_to_move });
        } else if (move.type() == MoveType::PromotionKnight) {
            addPiece(move.to(), Piece{ Piece::Knight, color_to_move });
        } else if (move.type() == MoveType::PromotionRook) {
            addPiece(move.to(), Piece{ Piece::Rook, color_to_move });
        }
        enpassant_square = -1;
    } else {

        if (move.type() == MoveType::Castling) {
            int rook_start_file = 0;
            int rook_target_file = 0;

            if (move.to() == Squares::G1 || move.to() == Squares::G8) {
                rook_start_file = 7;
                rook_target_file = 5;
            } else {
                rook_start_file = 0;
                rook_target_file = 3;
            }

            if (whiteToMove()) {
                castling_rights &= ~(white_queen_castle | white_king_castle);
            } else {
                castling_rights &= ~(black_queen_castle | black_king_castle);
            }

            int rank = rankOf(move.to());
            int rook_start = makeSquare(rook_start_file, rank);
            int rook_target = makeSquare(rook_target_file, rank);
            movePiece(rook_start, rook_target);
        }

        if (move.type() == MoveType::EnPassant) {
            int enpassant = 0;
            if (whiteToMove()) {
                enpassant = move.to() - BoardLength;
            } else {
                enpassant = move.to() + BoardLength;
            }

            removePiece(enpassant);
        }

        if (move.type() == MoveType::DoublePush) {
            if (whiteToMove()) {
                enpassant_square = move.to() - BoardLength;
            } else {
                enpassant_square = move.to() + BoardLength;
            }
        } else {
            enpassant_square = -1;
        }

        movePiece(move.from(), move.to());
    }

    color_to_move = Piece::flipColor(color_to_move);

    if (castling_rights != new_state.castling_rights) {
        hash ^= hashes.castling_rights[new_state.castling_rights];
        hash ^= hashes.castling_rights[castling_rights];
    }

    if (new_state.enpassant_square != -1) {
        hash ^= hashes.enpassant_file[fileOf(new_state.enpassant_square)];
    }

    if (enpassant_square != -1) {
        hash ^= hashes.enpassant_file[fileOf(enpassant_square)];
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
            int rook_start_file = 0;
            int rook_target_file = 0;

            if (fileOf(move.to()) == 6) {
                rook_start_file = 7;
                rook_target_file = 5;
            } else {
                rook_start_file = 0;
                rook_target_file = 3;
            }

            int rank = rankOf(move.to());
            int rook_start = makeSquare(rook_start_file, rank);
            int rook_target = makeSquare(rook_target_file, rank);
            movePiece(rook_target, rook_start);
        }

        if (move.type() == MoveType::EnPassant) {
            int enpassant = 0;
            if (whiteToMove()) {
                enpassant = move.to() - BoardLength;
            } else {
                enpassant = move.to() + BoardLength;
            }

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