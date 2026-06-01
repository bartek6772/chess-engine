#pragma once
#include "board.hpp"
#include "constants.hpp"
#include "move_list.hpp"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>

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
            int square = row * BoardLength + col;

            bool containing = std::ranges::find_if(moves.begin(), moves.end(), [square](Move m) {
                return m.to() == square;
            }) != moves.end();

            std::cout << std::setw(2) << (containing ? "1" : "0");
        }
        std::cout << std::endl;
    }
}

inline void printBoard(const Board& board) {
    for (int row = BoardLength - 1; row >= 0; row--) {
        for (int col = 0; col < BoardLength; col++) {
            int square = row * BoardLength + col;
            char symbol = board.squares[square].getSymbol();
            std::cout << std::setw(2) << symbol;
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

inline std::string readableNumber(unsigned long long number) {
    std::string result;

    int counter = 0;
    while (number > 0) {
        int digit = number % 10;
        number /= 10;

        if (counter >= 3) {
            counter = 0;
            result += ",";
        }

        result += ('0' + digit);
        counter++;
    }

    std::reverse(result.begin(), result.end());
    return result;
}