#include "board.hpp"
#include "constants.hpp"
#include "move.hpp"
#include "move_generator.hpp"
#include "move_list.hpp"
#include "pieces.hpp"
#include "searcher.hpp"
#include <algorithm>
#include <iomanip>
#include <ios>
#include <iostream>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <thread>

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

Move parseMove(Board& board, const std::string& move_str) {
    int from = (move_str[0] - 'a') + (move_str[1] - '1') * BoardLength;
    int to = (move_str[2] - 'a') + (move_str[3] - '1') * BoardLength;

    MoveType type = MoveType::Normal;
    bool is_promotion = move_str.size() == 5;
    if (is_promotion) {
        switch (move_str[4]) {
            case 'q': type = MoveType::PromotionQueen; break;
            case 'n': type = MoveType::PromotionKnight; break;
            case 'b': type = MoveType::PromotionBishop; break;
            case 'r': type = MoveType::PromotionRook; break;
        }
    }

    MoveList moves = MoveGenerator::generateLegalMoves(board);
    for (const Move& move : moves) {

        if (move.from == from && move.to == to) {
            if (is_promotion) {
                if (move.type == type) return move;
            } else {
                return move;
            }
        }
    }
    return {};
}

void position(Board& board, std::stringstream& stream) {
    using namespace std;

    string token;
    stream >> token;

    if (token == "startpos") {
        board.loadStartPos();
    } else if (token == "fen") {
        string fen;
        while (stream >> token && token != "moves") {
            fen += (fen.empty() ? "" : " ") + token;
        }
        board.loadFEN(fen);
    }

    if (token != "moves") {
        stream >> token;
    }

    if (token == "moves") {
        string move_str;
        while (stream >> move_str) {
            Move move = parseMove(board, move_str);
            board.makeMove(move);
        }
    }
}

// std::atomic<bool> is_searching = false;
// std::atomic<bool> stop_search = false;
std::unique_ptr<Searcher> current_search = nullptr;

void handleInput(std::string& input, Board& board) {
    using namespace std;

    stringstream stream(input);
    string command;
    stream >> command;

    if (command == "uci") {
        cout << "id name MyChessEngine v0.1" << endl;
        cout << "id author Bartłomiej Borowski" << endl;
        cout << "uciok" << endl;
    } else if (command == "isready") {
        cout << "readyok" << endl;
    } else if (command == "position") {
        position(board, stream);
    } else if (command == "legalmoves") {
        MoveList moves = MoveGenerator::generateLegalMoves(board);
        for (Move move : moves) {
            cout << move.toString() << endl;
        }
        cout << "done" << endl;
    } else if (command == "quit") {
        // stop_search = true;
        current_search->stop();
        exit(0);
    } else if (command == "go") {

        // TODO: add check to avoid starting new search during previous one
        // consider ways to destroy searcher after it finish searching

        // if (is_searching) return;
        // is_searching = true;
        // stop_search = false;

        int depth = 4;
        int move_time = 1000;
        string part;
        while (stream >> part) {
            if (part == "depth") {
                stream >> depth;
            } else if (part == "movetime") {
                stream >> move_time;
            }
        }

        Board board_copy = board;
        current_search = make_unique<Searcher>(board_copy);

        thread search_thread([depth, move_time]() {
            auto result = current_search->findBestMove(depth, move_time);
            cout << "bestmove " << result.best_move.toString() << endl;
            // is_searching = false;
        });
        search_thread.detach();

    } else if (command == "stop") {
        if (current_search) current_search->stop();
        // stop_search = true;
    } else if (command == "ucinewgame") {

    }

    else if (command == "d") {
        printBoard(board);
    }
}

auto main() -> int {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    Board board;
    // board.loadStartPos();

    std::string line;
    while (std::getline(std::cin, line)) {
        handleInput(line, board);
    }
    return 0;
}