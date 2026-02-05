#include "move_generator.hpp"
#include "board.hpp"
#include "constants.hpp"
#include "move.hpp"
#include "pieces.hpp"
#include <bit>
#include <vector>

struct ScanData {
    int row, col;
    int enemy_color;
    int dir_row, dir_col;
};

void scanDir(const Board& board, std::vector<Move>& moves, const ScanData& data) {
    int r = data.row + data.dir_row;
    int c = data.col + data.dir_col;

    int from = data.row * BoardLength + data.col;

    while (r >= 0 && r < BoardLength && c >= 0 && c < BoardLength) {

        int target = r * BoardLength + c;
        if (board.squares[target] == Pieces::None) {
            moves.push_back({ from, target });
        } else if (Pieces::pieceColor(board.squares[target]) == data.enemy_color) {
            moves.push_back({ from, target });
            break;
        } else {
            break;
        }

        r += data.dir_row;
        c += data.dir_col;
    }
}

void MoveGenerator::generateKnightMoves(const Board& board, std::vector<Move>& moves) {
    int piece = board.white_to_move ? Pieces::WhiteKnight : Pieces::BlackKnight;
    bitmask friendly_pieces = board.white_to_move ? board.white_pieces : board.black_pieces;
    for (int from : board.pieceLists[piece]) {
        bitmask mask = precomputed.knightMoves[from] & ~friendly_pieces;

        while (mask > 0) {
            int target = std::countr_zero(mask);
            moves.push_back({ from, target });
            mask &= (mask - 1);
        }
    }
}

void MoveGenerator::generateRookMoves(const Board& board, std::vector<Move>& moves) {
    int piece = board.white_to_move ? Pieces::WhiteRook : Pieces::BlackRook;
    int enemy_color = board.white_to_move ? Pieces::Black : Pieces::White;

    for (int from : board.pieceLists[piece]) {
        int row = from / BoardLength;
        int col = from % BoardLength;

        scanDir(board, moves, { row, col, enemy_color, 1, 0 });
        scanDir(board, moves, { row, col, enemy_color, -1, 0 });
        scanDir(board, moves, { row, col, enemy_color, 0, 1 });
        scanDir(board, moves, { row, col, enemy_color, 0, -1 });
    }
}

void MoveGenerator::generateBishopMoves(const Board& board, std::vector<Move>& moves) {
    int piece = board.white_to_move ? Pieces::WhiteBishop : Pieces::BlackBishop;
    int enemy_color = board.white_to_move ? Pieces::Black : Pieces::White;

    for (int from : board.pieceLists[piece]) {
        int row = from / BoardLength;
        int col = from % BoardLength;

        scanDir(board, moves, { row, col, enemy_color, 1, 1 });
        scanDir(board, moves, { row, col, enemy_color, -1, 1 });
        scanDir(board, moves, { row, col, enemy_color, -1, -1 });
        scanDir(board, moves, { row, col, enemy_color, 1, -1 });
    }
}

void MoveGenerator::generateQueenMoves(const Board& board, std::vector<Move>& moves) {
    int piece = board.white_to_move ? Pieces::WhiteQueen : Pieces::BlackQueen;
    int enemy_color = board.white_to_move ? Pieces::Black : Pieces::White;

    for (int from : board.pieceLists[piece]) {
        int row = from / BoardLength;
        int col = from % BoardLength;

        scanDir(board, moves, { row, col, enemy_color, 1, 0 });
        scanDir(board, moves, { row, col, enemy_color, -1, 0 });
        scanDir(board, moves, { row, col, enemy_color, 0, 1 });
        scanDir(board, moves, { row, col, enemy_color, 0, -1 });

        scanDir(board, moves, { row, col, enemy_color, 1, 1 });
        scanDir(board, moves, { row, col, enemy_color, -1, 1 });
        scanDir(board, moves, { row, col, enemy_color, -1, -1 });
        scanDir(board, moves, { row, col, enemy_color, 1, -1 });
    }
}

auto MoveGenerator::generateMoves(const Board& board) -> std::vector<Move> {
    std::vector<Move> moves;

    generateKnightMoves(board, moves);
    generateRookMoves(board, moves);
    generateBishopMoves(board, moves);
    generateQueenMoves(board, moves);

    return moves;
}