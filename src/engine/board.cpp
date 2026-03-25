#include "board.hpp"
#include "constants.hpp"
#include "move.hpp"
#include "pieces.hpp"
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

Board::Board() {
    history.reserve(100);
}

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
    bitboards[piece] ^= 1LL << square;

    if (Pieces::isWhite(piece)) {
        white_pieces ^= 1LL << square;
    } else {
        black_pieces ^= 1LL << square;
    }
}

// does not handle captures
void Board::movePiece(int from, int to) {
    int piece = squares[from];

    squares[from] = Pieces::None;
    squares[to] = piece;

    bitboards[piece] ^= (1LL << from) | (1LL << to);
    pieceLists[piece].movePiece(from, to);

    if (Pieces::isWhite(piece)) {
        white_pieces ^= (1LL << from) | (1LL << to);
    } else {
        black_pieces ^= (1LL << from) | (1LL << to);
    }
}

auto Board::loadFEN(const std::string& fen) -> bool {

    // TODO: handle errors and invalid strings, maybe return bool as veryfication

    history.clear();

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
                int piece = Pieces::getPiece(c);
                addPiece(row * BoardLength + col, piece);
                col++;
            }
        }
    }

    { // 2. Side to move
        if (parts.size() < 2) {
            return false;
        }

        if (parts[1] == "w") {
            white_to_move = true;
        } else if (parts[1] == "b") {
            white_to_move = false;
        } else {
            return false;
        }
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

void Board::makeMove(const Move& move) {

    HistoryState new_state{ move };
    new_state.enpassant_square = enpassant_square;
    new_state.castling_rights = castling_rights;

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
        enpassant_square = -1;
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

            if (white_to_move) {
                castling_rights &= ~(white_queen_castle | white_king_castle);
            } else {
                castling_rights &= ~(black_queen_castle | black_king_castle);
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
            if (white_to_move) {
                enpassant_square = move.to - BoardLength;
            } else {
                enpassant_square = move.to + BoardLength;
            }
        } else {
            enpassant_square = -1;
        }

        if (move.from == 0) {
            castling_rights &= ~white_queen_castle;
        } else if (move.from == 7) {
            castling_rights &= ~white_king_castle;
        } else if (move.from == 56) {
            castling_rights &= ~black_queen_castle;
        } else if (move.from == 63) {
            castling_rights &= ~black_king_castle;
        }

        int capture = squares[move.to];
        if (capture != Pieces::None) {
            removePiece(move.to);
        }
        new_state.capture = capture;

        movePiece(move.from, move.to);
    }

    white_to_move = !white_to_move;
    history.push_back(new_state);
}

void Board::unmakeMove() {

    HistoryState state = history[history.size() - 1];
    history.pop_back();
    Move& move = state.move;

    white_to_move = !white_to_move;
    enpassant_square = state.enpassant_square;
    castling_rights = state.castling_rights;

    bool was_promotion =
        move.type == MoveType::PromotionBishop || move.type == MoveType::PromotionKnight ||
        move.type == MoveType::PromotionQueen || move.type == MoveType::PromotionRook;

    int color = white_to_move ? Pieces::White : Pieces::Black;

    if (was_promotion) {
        removePiece(move.to);
        addPiece(move.from, Pieces::Pawn | color);
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
            movePiece(rook_target, rook_start);
        }

        if (move.type == MoveType::EnPassant) {
            int enpassant = 0;
            if (white_to_move) {
                enpassant = move.to - BoardLength;
            } else {
                enpassant = move.to + BoardLength;
            }

            addPiece(enpassant, white_to_move ? Pieces::BlackPawn : Pieces::WhitePawn);
        }

        movePiece(move.to, move.from);
        if (state.capture != Pieces::None) {
            addPiece(move.to, state.capture);
        }
    }
}
