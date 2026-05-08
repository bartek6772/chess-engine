#pragma once
#include "board.hpp"
#include "constants.hpp"
#include "move_list.hpp"
#include "pieces.hpp"
#include <iomanip>
#include <iostream>

inline void printBitboard(unsigned long long moves) {
    for (int row = BoardLength - 1; row >= 0; row--) {
        for (int col = 0; col < BoardLength; col++) {
            int bit = row * BoardLength + col;
            std::cout << std::setw(2) << (moves & (1ULL << bit) ? "1" : "0");
        }
        std::cout << std::endl;
    }
}

inline void printMoves(const MoveList& moves) {
    for (int row = BoardLength - 1; row >= 0; row--) {
        for (int col = 0; col < BoardLength; col++) {
            int bit = row * BoardLength + col;

            bool containing = std::ranges::find_if(moves.begin(), moves.end(), [bit](Move m) {
                return m.to() == bit;
            }) != moves.end();

            std::cout << std::setw(2) << (containing ? "1" : "0");
        }
        std::cout << std::endl;
    }
}

inline void printBoard(const Board& board) {
    for (int row = BoardLength - 1; row >= 0; row--) {
        for (int col = 0; col < BoardLength; col++) {
            int bit = row * BoardLength + col;
            char symbol = Pieces::getSymbol(board.squares[bit]);
            std::cout << std::setw(2) << symbol;
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}