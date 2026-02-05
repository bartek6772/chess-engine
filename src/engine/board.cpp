#include "board.hpp"
#include "constants.hpp"
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

auto Board::loadFEN(const std::string& fen) -> bool {

    // TODO: handle errors and invalid strings, maybe return bool as veryfication

    std::vector<std::string> parts;
    std::stringstream stream(fen);
    std::string str;

    while (std::getline(stream, str, ' ')) {
        parts.push_back(str);
    }

    if (parts.size() > 6) { return false; }

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
