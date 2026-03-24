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

static void scanDir(const Board& board, std::vector<Move>& moves, const ScanData& data) {
    int r = data.row + data.dir_row;
    int c = data.col + data.dir_col;

    int from = data.row * BoardLength + data.col;

    while (r >= 0 && r < BoardLength && c >= 0 && c < BoardLength) {

        int target = r * BoardLength + c;
        if (board.squares[target] == Pieces::None) {
            moves.emplace_back(from, target);
        } else if (Pieces::pieceColor(board.squares[target]) == data.enemy_color) {
            moves.emplace_back(from, target);
            break;
        } else {
            break;
        }

        r += data.dir_row;
        c += data.dir_col;
    }
}

void MoveGenerator::generateKnightMoves(
    const Board& board, std::vector<Move>& moves, int color) const {
    int piece = Pieces::Knight | color;
    bitmask friendly_pieces = color == Pieces::White ? board.white_pieces : board.black_pieces;
    for (int from : board.pieceLists[piece]) {
        bitmask mask = precomputed.knightMoves[from] & ~friendly_pieces;

        while (mask > 0) {
            int target = std::countr_zero(mask);
            moves.emplace_back(from, target);
            mask &= (mask - 1);
        }
    }
}

void MoveGenerator::generateRookMoves(
    const Board& board, std::vector<Move>& moves, int color) const {
    int piece = Pieces::Rook | color;
    int enemy_color = color == Pieces::White ? Pieces::Black : Pieces::White;

    for (int from : board.pieceLists[piece]) {
        int row = from / BoardLength;
        int col = from % BoardLength;

        scanDir(board, moves, { row, col, enemy_color, 1, 0 });
        scanDir(board, moves, { row, col, enemy_color, -1, 0 });
        scanDir(board, moves, { row, col, enemy_color, 0, 1 });
        scanDir(board, moves, { row, col, enemy_color, 0, -1 });
    }
}

void MoveGenerator::generateBishopMoves(
    const Board& board, std::vector<Move>& moves, int color) const {
    int piece = Pieces::Bishop | color;
    int enemy_color = color == Pieces::White ? Pieces::Black : Pieces::White;

    for (int from : board.pieceLists[piece]) {
        int row = from / BoardLength;
        int col = from % BoardLength;

        scanDir(board, moves, { row, col, enemy_color, 1, 1 });
        scanDir(board, moves, { row, col, enemy_color, -1, 1 });
        scanDir(board, moves, { row, col, enemy_color, -1, -1 });
        scanDir(board, moves, { row, col, enemy_color, 1, -1 });
    }
}

void MoveGenerator::generateQueenMoves(
    const Board& board, std::vector<Move>& moves, int color) const {
    int piece = Pieces::Queen | color;
    int enemy_color = color == Pieces::White ? Pieces::Black : Pieces::White;

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

void MoveGenerator::generatePawnMoves(
    const Board& board, std::vector<Move>& moves, int color) const {
    int piece = Pieces::Pawn | color;
    int enemy_color = piece == Pieces::White ? Pieces::Black : Pieces::White;

    bool white_to_move = color == Pieces::White;
    int dir = white_to_move ? 1 : -1;
    int base_row = white_to_move ? 1 : 6;
    int promotion_row = white_to_move ? 7 : 0;

    int left_attack = white_to_move ? 7 : -9;
    int right_attack = white_to_move ? 9 : -7;

    constexpr int double_push = 16;
    constexpr int push = 8;

    for (int from : board.pieceLists[piece]) {
        int from_row = from / BoardLength;
        int from_col = from % BoardLength;

        if (from_col != 0) {
            int target = from + left_attack;
            if (board.squares[target] != Pieces::None &&
                Pieces::pieceColor(board.squares[target]) == enemy_color) {
                moves.emplace_back(from, target);
            }

            if (board.enpassant_square == target) {
                moves.emplace_back(from, target, MoveType::EnPassant);
            }
        }
        if (from_col != BoardLength - 1) {
            int target = from + right_attack;
            if (board.squares[target] != Pieces::None &&
                Pieces::pieceColor(board.squares[target]) == enemy_color) {
                moves.emplace_back(from, target);
            }

            if (board.enpassant_square == target) {
                moves.emplace_back(from, target, MoveType::EnPassant);
            }
        }

        int push1 = from + dir * push;
        int push2 = from + dir * double_push;

        bool empty1 = board.squares[push1] == Pieces::None;
        bool empty2 = board.squares[push2] == Pieces::None;

        if (empty1) {
            if (push1 / BoardLength == promotion_row) {
                moves.emplace_back(from, push1, MoveType::PromotionBishop);
                moves.emplace_back(from, push1, MoveType::PromotionKnight);
                moves.emplace_back(from, push1, MoveType::PromotionQueen);
                moves.emplace_back(from, push1, MoveType::PromotionRook);
            } else {
                moves.emplace_back(from, push1);
            }
        }

        if (from_row == base_row) {
            if (empty1 && empty2) {
                moves.emplace_back(from, push2, MoveType::DoublePush);
            }
        }
    }
}

void MoveGenerator::generateKingMoves(
    const Board& board, std::vector<Move>& moves, int color) const {

    // TODO: try to ignore attacked squares to avoid checking them later
    int piece = Pieces::King | color;
    bitmask friendly_pieces = color == Pieces::White ? board.white_pieces : board.black_pieces;
    for (int from : board.pieceLists[piece]) {
        bitmask mask = precomputed.kingMoves[from] & ~friendly_pieces;

        while (mask > 0) {
            int target = std::countr_zero(mask);
            moves.emplace_back(from, target);
            mask &= (mask - 1);
        }
    }

    // TODO: implement castling
}

bool MoveGenerator::isSquareAttacked(const Board& board, int square) {
    if (board.white_to_move) {
        return (black_attacks & (1LL << square)) != 0;
    } else {
        return (white_attacks & (1LL << square)) != 0;
    }
}

void MoveGenerator::generateAttacks(const Board& board) {
    // TODO: Change later to bitbords

    std::vector<Move> moves;
    generateKnightMoves(board, moves, Pieces::Black);
    generateRookMoves(board, moves, Pieces::Black);
    generateBishopMoves(board, moves, Pieces::Black);
    generateQueenMoves(board, moves, Pieces::Black);
    generatePawnMoves(board, moves, Pieces::Black);
    generateKingMoves(board, moves, Pieces::Black);

    for (const Move& move : moves) {
        black_attacks |= (1LL << move.to);
    }
    moves.clear();

    generateKnightMoves(board, moves, Pieces::White);
    generateRookMoves(board, moves, Pieces::White);
    generateBishopMoves(board, moves, Pieces::White);
    generateQueenMoves(board, moves, Pieces::White);
    generatePawnMoves(board, moves, Pieces::White);
    generateKingMoves(board, moves, Pieces::White);

    for (const Move& move : moves) {
        white_attacks |= (1LL << move.to);
    }
}

auto MoveGenerator::generateMoves(const Board& board) -> std::vector<Move> {
    std::vector<Move> moves;

    generateAttacks(board);

    int color = board.white_to_move ? Pieces::White : Pieces::Black;
    generateKnightMoves(board, moves, color);
    generateRookMoves(board, moves, color);
    generateBishopMoves(board, moves, color);
    generateQueenMoves(board, moves, color);
    generatePawnMoves(board, moves, color);
    generateKingMoves(board, moves, color);

    return moves;
}

auto MoveGenerator::generateLegalMoves(Board& board) -> std::vector<Move> {
    std::vector<Move> legal_moves;
    int color = board.white_to_move ? Pieces::White : Pieces::Black;

    std::vector<Move> moves = generateMoves(board);
    for (const Move& move : moves) {
        board.makeMove(move);
        int king_position = board.pieceLists[Pieces::King | color][0];
        bool legal_move = true;

        std::vector<Move> responses = generateMoves(board);
        for (const Move& response : responses) {
            if (response.to == king_position) {
                legal_move = false;
                break;
            }
        }

        if (legal_move) {
            legal_moves.push_back(move);
        }

        board.unmakeMove();
    }

    return legal_moves;
}
