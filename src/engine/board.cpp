#include "board.hpp"
#include "constants.hpp"
#include "move.hpp"
#include "pieces.hpp"
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

void Board::addPiece(int square, int piece) {
    squares[square] = piece;
    pieceLists[piece].addPiece(square);
    bitboards[piece] |= 1LL << square;

    if (Pieces::isWhite(piece)) {
        white_pieces |= 1LL << square;
    } else {
        black_pieces |= 1LL << square;
    }
}

void Board::removePiece(int square) {
    int piece = squares[square];
    squares[square] = Pieces::None;
    pieceLists[piece].removePiece(square);
    bitboards[piece] ^= 1 << square;

    if (Pieces::isWhite(piece)) {
        white_pieces ^= 1 << square;
    } else {
        black_pieces ^= 1 << square;
    }
}

// does not handle captures
void Board::movePiece(int from, int to) {
    int piece = squares[from];

    squares[from] = Pieces::None;
    squares[to] = piece;

    bitboards[piece] ^= (1 << from) & (1 << to);
    pieceLists[piece].movePiece(from, to);
}

auto Board::loadFEN(const std::string& fen) -> bool {

    // TODO: handle errors and invalid strings, maybe return bool as veryfication

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
        int row = BoardLength - 1;
        int col = 0;

        for (char c : parts[0]) {
            if (std::isdigit(c)) {
                col += c;
            } else if (c == '/') {
                row -= 1;
                col = 0;
            } else {
                int piece = Pieces::getPiece(c);
                addPiece(row * BoardLength + col, piece);
                col++;
            }
        }
    }

    { // 2. Side to move
        if (parts[1] == "w")
            white_to_move = true;
        else if (parts[1] == "b")
            white_to_move = false;
        else
            return false;
    }

    { // 3. Castling rights
    }

    { // 4. Enpassant
    }

    { // 5. 50 moves rule counter
    }

    { // 6. Total moves counter
    }

    return true;
}

void Board::makeMove(Move& move) {
    bool is_promotion =
        move.type == MoveType::PromotionBishop || move.type == MoveType::PromotionKnight ||
        move.type == MoveType::PromotionQueen || move.type == MoveType::PromotionRook;

    int color = white_to_move ? Pieces::White : Pieces::Black;

    if (is_promotion) {
        removePiece(move.from);
        if (move.type == MoveType::PromotionBishop) {
            addPiece(move.to, Pieces::Bishop | color);
        } else if (move.type == MoveType::PromotionQueen) {
            addPiece(move.to, Pieces::Queen | color);
        } else if (move.type == MoveType::PromotionKnight) {
            addPiece(move.to, Pieces::Knight | color);
        } else if (move.type == MoveType::PromotionRook) {
            addPiece(move.to, Pieces::Rook | color);
        }
    } else {

        if (move.type == MoveType::Castling) {
            int rook_start_file = 0;
            int rook_target_file = 0;

            if ((move.to % BoardLength) == 6) {
                rook_start_file = 7;
                rook_target_file = 5;
            } else {
                rook_start_file = 0;
                rook_target_file = 3;
            }

            int rank = move.to / BoardLength;
            int rook_start = rank * BoardLength + rook_start_file;
            int rook_target = rank * BoardLength + rook_target_file;
            movePiece(rook_start, rook_target);
        }

        if (move.type == MoveType::EnPassant) {
            int enpassant = 0;
            if (white_to_move) {
                enpassant = move.to - BoardLength;
            } else {
                enpassant = move.to + BoardLength;
            }

            removePiece(enpassant);
        }

        if (move.type == MoveType::DoublePush) {
            // enpassant_square = move.to - (white_to_move ? BoardLength : -BoardLength);
            if (white_to_move) {
                enpassant_square = move.to - BoardLength;
            } else {
                enpassant_square = move.to + BoardLength;
            }
        }

        int capture = squares[move.to];
        if (capture != Pieces::None) {
            removePiece(move.to);
        }

        movePiece(move.from, move.to);
    }
}
