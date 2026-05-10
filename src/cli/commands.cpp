#include "cli.hpp"
#include "constants.hpp"
#include "move_generator.hpp"
#include "searcher.hpp"
#include "utils.hpp"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

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

        if (move.from() == from && move.to() == to) {
            if (is_promotion) {
                if (move.type() == type) return move;
            } else {
                return move;
            }
        }
    }
    return {};
}

void CLI::uci(stringstream& stream) {
    cout << "id name MyChessEngine " << engine_version << endl;
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
        stream >> token;
    } else if (token == "fen") {
        string fen;
        while (stream >> token && token != "moves") {
            fen += (fen.empty() ? "" : " ") + token;
        }
        board.loadFEN(fen);
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

    stop(stream);

    constexpr int BUFFER = 50;
    constexpr int MINIMUM = 20;
    constexpr int ONE_HOUR = 60 * 60 * 1000;

    int depth = 0;
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
    }

    int time = 0;
    if (move_time != 0) {
        time = move_time;
    } else if (wtime != 0 || btime != 0) {
        int remaining_moves = max(20, 40 - board.history_ptr);

        int clock = board.white_to_move ? wtime : btime;
        int inc = board.white_to_move ? wtime : btime;

        int target = (clock + (remaining_moves - 1) * inc) / remaining_moves;
        int hard_limit = clock / 5;

        target = min(target, hard_limit) - BUFFER;
        time = max(target, MINIMUM);

    } else {
        time = ONE_HOUR;
    }

    if (depth == 0) depth = MaxSearchDepth;

    current_search = make_unique<Searcher>(board);
    current_search->enableInfo();

    search_thread = thread([this, depth, time]() {
        auto result = current_search->findBestMove(depth, time);
        if (!result.pv.empty()) {
            cout << "bestmove " << result.best_move.toString() << endl;
        }
    });
}

void CLI::stop(stringstream& stream) {
    if (current_search) {
        current_search->stop();
    }

    if (search_thread.joinable()) {
        search_thread.join();
    }
}

void CLI::quit(stringstream& stream) {
    exit(0);
}

void CLI::bench(stringstream& stream) {
    static const vector<string> positions = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
        "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",
        "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
        "8/k7/3p4/p2P1p2/P2P1P2/8/8/K7 w - - 0 1",
    };

    constexpr int default_depth = 8;
    constexpr int time = 60 * 1000;
    int depth = default_depth;
    stream >> depth;

    cout << "Running benchmark - " << positions.size() << " positions, depth " << depth << "\n\n";
    Board bench_board;

    unsigned long long nodes_total = 0;
    long time_total = 0;

    for (const string& fen : positions) {
        cout << "Position: " << fen << endl;

        bench_board.loadFEN(fen);

        Searcher search(bench_board);
        SearchResult results = search.findBestMove(depth, time);

        if (results.stats.time_ms > 0) {
            nodes_total += results.stats.nodes;
            time_total += results.stats.time_ms;
        }

        cout << "Depth: " << depth << ", Nodes: " << results.stats.nodes
             << ", Time: " << results.stats.time_ms << ", NPS: " << results.stats.nodes_per_second
             << ", TT usage: " << Searcher::tableFillRate() << endl;
        cout << endl;

        Searcher::clearTable();
    }

    unsigned long long nps = nodes_total * 1000 / time_total;

    constexpr int label_width = 25;
    constexpr int value_width = 14;

    cout << setfill('=') << setw(label_width + value_width + 1) << "" << endl;
    cout << setfill(' ');

    cout << left << setw(label_width) << "Total nodes searched" << ":" << right << setw(value_width)
         << readableNumber(nodes_total) << endl;
    cout << left << setw(label_width) << "Total time" << ":" << right << setw(value_width)
         << readableNumber(time_total) << endl;
    cout << left << setw(label_width) << "Nodes per second" << ":" << right << setw(value_width)
         << readableNumber(nps) << endl;

    cout << setfill('=') << setw(label_width + value_width + 1) << "" << endl;
    cout << setfill(' ');
}