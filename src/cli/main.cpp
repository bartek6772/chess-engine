#include "board.hpp"
#include "constants.hpp"
#include "diagnostics.hpp"
#include "magics.hpp"
#include "move.hpp"
#include "move_generator.hpp"
#include "pieces.hpp"
#include "precomputed.hpp"
#include <algorithm>
#include <iomanip>
#include <ios>
#include <iostream>
#include <ostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#define START_POS "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

std::string last_loaded_position;

void printBitboard(unsigned long long moves) {
    for (int row = BoardLength - 1; row >= 0; row--) {
        for (int col = 0; col < BoardLength; col++) {
            int bit = row * BoardLength + col;
            std::cout << std::setw(2) << (moves & (1ULL << bit) ? "1" : "0");
        }
        std::cout << std::endl;
    }
}

void printMoves(const MoveList& moves) {
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
    std::cout << std::endl;
}

void handleInput(std::string& input, Board& board, MoveGenerator& move_generator) {
    using namespace std;

    constexpr int TEST_CASES = 6;

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

            position += sth + " ";
        }

        if (position != last_loaded_position) {
            cout << "Loading fen: " << position << endl;
            bool success = board.loadFEN(position);
            if (!success) {
                cout << "failed" << endl;
                return;
            }
            last_loaded_position = position;
        }

        int move_index = 0;
        while (stream >> sth) {

            if (sth == "moves") {
                continue;
            }

            // cant make it this way because it loses flag, need to find move in generated
            auto find_move = [&](int from, int to) {
                MoveList moves = move_generator.generateMoves(board);
                Move move(0, 0);
                for (const Move& m : moves) {
                    if (m.from == from && m.to == to) {
                        return m;
                    }
                }
                return move;
            };

            Move sth_move(sth);

            if (board.history.size() >= move_index + 1) {

                if (board.history[move_index].move.from != sth_move.from ||
                    board.history[move_index].move.to != sth_move.to) {

                    while (board.history.size() - 1 >= move_index) {
                        board.unmakeMove();
                    }

                    board.makeMove(find_move(sth_move.from, sth_move.to));
                }
            } else {
                board.makeMove(find_move(sth_move.from, sth_move.to));
            }

            move_index++;
        }

    } else if (command == "legalmoves") {
        // change back to legal moves
        MoveList moves = move_generator.generateMoves(board);
        for (Move move : moves) {
            cout << move.toString() << endl;
        }
        cout << "done" << endl;
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

    // else if (command == "test") {
    //     printBoard(board);

    //     for (int i = 0; i < TEST_CASES; i++) {
    //         MoveList moves = move_generator.generateLegalMoves(board);
    //         std::random_device dev;
    //         std::mt19937 rng(dev());

    //         if (moves.size() > 0) {
    //             std::uniform_int_distribution<std::mt19937::result_type> dist(0, moves.size() -
    //             1); board.makeMove(moves[dist(rng)]);

    //             // bug here

    //             printBoard(board);
    //         }
    //     }
    // }

    else if (command == "undo_test") {
        cout << "\nUNMAKING MOVES\n";
        for (int i = 0; i < TEST_CASES; i++) {
            board.unmakeMove();
        }
    }
}

auto main() -> int {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    Board board;
    Precomputed precomputed;
    MoveGenerator moveGenerator(precomputed);

    board.loadFEN(START_POS);

    // std::string line;
    // while (std::getline(std::cin, line)) {
    //     handleInput(line, board, moveGenerator);
    // }

    // Magics magics;
    // for (auto magic : magics.rook_magics) {
    //     std::cout << magic << std::endl;
    // }

    // std::cout << Diagnostics::runPerft(board, moveGenerator, 2) << std::endl;

    // board.loadFEN(START_POS);
    // board.makeMove(Move("a2a4"));
    // board.makeMove({ 8, 24, MoveType::DoublePush });
    // board.makeMove({ 48, 40 });
    // board.makeMove({ 24, 32 });
    // board.makeMove({ 49, 49 - 16, MoveType::DoublePush });

    // printBoard(board);
    // std::cout << board.enpassant_square << "\n";
    // printBitboard(1ULL << board.enpassant_square);

    // board.loadFEN("position startpos moves a2a4 a7a6 a4a5 b7b5");
    // MoveList moves = moveGenerator.generateMoves(board);
    // for (Move move : moves) {
    //     std::cout << move.toString() << std::endl;
    // }

    return 0;
}