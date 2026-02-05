#include "board.hpp"
#include "pieces.hpp"

void Board::addPiece(int square, int piece) {
    squares[square] = piece;
    pieceLists[piece].addPiece(square);
}

void Board::removePiece(int square) {
    int piece = squares[square];
    squares[square] = Pieces::None;
    pieceLists[piece].removePiece(square);
}
