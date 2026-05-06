#include "cli.hpp"
#include "move_generator.hpp"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <thread>

using namespace std;

Move parseMove(Board& board, const string& move_str) {
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

void CLI::uci(stringstream& stream) {
    cout << "id name MyChessEngine v0.1" << endl;
    cout << "id author Bartłomiej Borowski" << endl;
    cout << "uciok" << endl;
}

void CLI::readyok(stringstream& stream) {
    cout << "readyok" << endl;
}

void CLI::ucinewgame(stringstream& stream) {
}

void CLI::position(stringstream& stream) {
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
            if (move.isNull()) {
                cout << "failed parsing move" << endl;
                continue;
            }
            board.makeMove(move);
        }
    }
}

void CLI::go(stringstream& stream) {

    if (is_searching) return;

    int depth = 64;
    int move_time = 0;
    int wtime = 0;
    int btime = 0;
    int winc = 0;
    int binc = 0;
    bool infinite = false;

    string part;
    while (stream >> part) {
        if (part == "depth") stream >> depth;
        else if (part == "movetime") stream >> move_time;
        else if (part == "wtime") stream >> wtime;
        else if (part == "btime") stream >> btime;
        else if (part == "winc") stream >> winc;
        else if (part == "binc") stream >> binc;
        else if (part == "infinite") infinite = true;
    }

    int time = move_time;
    if (move_time == 0) {
        int remaining = board.white_to_move ? wtime : btime;
        int inc = board.white_to_move ? winc : binc;
        time = (remaining / 40) + (inc * 0.8);
    }

    if (infinite || time == 0) {
        time = 60 * 60 * 1000; // one hour
    }

    if (time > 100) time -= 50;

    current_search = make_unique<Searcher>(board);
    current_search->enableInfo();
    is_searching = true;

    thread search_thread([this, depth, time]() {
        auto result = current_search->findBestMove(depth, time);
        if (!result.pv.empty()) {
            cout << "bestmove " << result.best_move.toString() << endl;
        }
        is_searching = false;
    });
    search_thread.detach();
}

void CLI::stop(stringstream& stream) {
    if (is_searching && current_search) {
        current_search->stop();
    }
}

void CLI::quit(stringstream& stream) {
    exit(0);
}