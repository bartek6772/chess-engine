#include "board.hpp"
#include "constants.hpp"
#include "move.hpp"
#include "move_generator.hpp"
#include "pieces.hpp"
#include "precomputed.hpp"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <vector>

void printBitboard(unsigned long long moves) {
    for (int row = BoardLength - 1; row >= 0; row--) {
        for (int col = 0; col < BoardLength; col++) {
            int bit = row * 8 + col;
            std::cout << std::setw(2) << (moves & (1LL << bit) ? "1" : "0");
        }
        std::cout << std::endl;
    }
}

void printMoves(const std::vector<Move>& moves) {
    for (int row = BoardLength - 1; row >= 0; row--) {
        for (int col = 0; col < BoardLength; col++) {
            int bit = row * 8 + col;

            bool containing = std::find_if(moves.begin(), moves.end(), [bit](Move m) {
                return m.to == bit;
            }) != moves.end();

            std::cout << std::setw(2) << (containing ? "1" : "0");
        }
        std::cout << std::endl;
    }
}

void printBoard(const Board& board) {
    for (int row = BoardLength - 1; row >= 0; row--) {
        for (int col = 0; col < BoardLength; col++) {
            int bit = row * 8 + col;
            char symbol = Pieces::getSymbol(board.squares[bit]);
            std::cout << std::setw(2) << symbol;
        }
        std::cout << std::endl;
    }
}

auto main() -> int {

    Board board;
    Precomputed precomputed;
    MoveGenerator moveGenerator(precomputed);

    std::cout << "Hello World from Ubuntu!" << std::endl;

    bool success = board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    if (!success) std::cout << "error while loading" << std::endl;

    printBoard(board);

    std::vector<Move> moves = moveGenerator.generateMoves(board);
    std::cout << "Moves count " << moves.size() << std::endl;

    // for (auto [f, t, s] : moves) {
    //     std::cout << f << ' ' << t << '\n';
    // }

    // printBitboard(precomputed.knightMoves[1] & ~board.white_pieces);
    // std::cout << std::endl;

    return 0;
}