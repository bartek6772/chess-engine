#include "board.hpp"
#include "constants.hpp"
#include "move.hpp"
#include "move_generator.hpp"
#include "pieces.hpp"
#include "precomputed.hpp"
#include <algorithm>
#include <iomanip>
#include <ios>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

void printBitboard(unsigned long long moves) {
    for (int row = BoardLength - 1; row >= 0; row--) {
        for (int col = 0; col < BoardLength; col++) {
            int bit = row * BoardLength + col;
            std::cout << std::setw(2) << (moves & (1LL << bit) ? "1" : "0");
        }
        std::cout << std::endl;
    }
}

void printMoves(const std::vector<Move>& moves) {
    for (int row = BoardLength - 1; row >= 0; row--) {
        for (int col = 0; col < BoardLength; col++) {
            int bit = row * BoardLength + col;

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
            int bit = row * BoardLength + col;
            char symbol = Pieces::getSymbol(board.squares[bit]);
            std::cout << std::setw(2) << symbol;
        }
        std::cout << std::endl;
    }
}

void handleInput(std::string& input, Board& board, MoveGenerator& move_generator) {
    using namespace std;

    std::stringstream stream(input);
    std::string command;
    stream >> command;

    if (command == "uci") {
        cout << "id name MyChessEngine v0.1" << endl;
        cout << "id author Bartłomiej Borowski" << endl;
        cout << "uciok" << endl;
    } else if (command == "isready") {
        cout << "readyok" << endl;
    } else if (command == "position") {

        string position{};
        string sth;

        while (stream >> sth) {
            if (sth == "moves") {
                break;
            }

            if (sth == "startpos") {
                position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
                break;
            }

            position += sth;
        }

        bool success = board.loadFEN(position);
        if (!success) {
            cout << "failed" << endl;
            return;
        }

        // read moves ...

    } else if (command == "legalmoves") {
        vector<Move> moves = move_generator.generateMoves(board);
        for (Move move : moves) {
            cout << move.toString() << endl;
        }
    } else if (command == "quit") {
        exit(0);
    }

    else if (command == "d") {
        printBoard(board);
    }

    // for debugging
    else if (command == "move") {
        string arg;
        stream >> arg;
        if (arg.size() < 4) {
            return;
        }
        Move move(arg);
        board.makeMove(move);
    }
}

auto main() -> int {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    Board board;
    Precomputed precomputed;
    MoveGenerator moveGenerator(precomputed);

    std::string line;
    while (std::getline(std::cin, line)) {
        handleInput(line, board, moveGenerator);
    }

    return 0;
}