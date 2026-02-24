#include "piece_list.hpp"

void PieceList::addPiece(int square) {
    list[count] = square;
    map[square] = count;
    count++;
}

void PieceList::removePiece(int square) {
    int index = map[square];
    int element_to_move = list[count - 1];

    list[index] = element_to_move;
    map[element_to_move] = index;

    count--;
}

void PieceList::movePiece(int from, int to) {
    int index = map[from];
    map[to] = index;
    list[index] = to;
}