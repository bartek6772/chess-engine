#include "move_generator.hpp"
#include "board.hpp"
#include "constants.hpp"
#include "magics.hpp"
#include "move.hpp"
#include "pieces.hpp"
#include "precomputed.hpp"
#include <bit>
#include <vector>

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

bitmask getRookAttack(const Board& board, const Magics& magics, int from) {
    int piece = board.squares[from];

    bitmask all_pieces = board.white_pieces | board.black_pieces;
    bitmask blockers = all_pieces & magics.getRookMask(from);
    bitmask targets = magics.getRookAttacks(from, blockers);

    return targets;
}

bitmask getBishopAttack(const Board& board, const Magics& magics, int from) {
    int piece = board.squares[from];

    bitmask all_pieces = board.white_pieces | board.black_pieces;
    bitmask blockers = all_pieces & magics.getBishopMask(from);
    bitmask targets = magics.getBishopAttacks(from, blockers);

    return targets;
}

bitmask getPawnsAttacks(const Board& board, int color) {
    bitmask attacks = 0;

    bitmask left_file_mask = 0x0101010101010101;
    bitmask right_file_mask = 0x8080808080808080;

    bitmask pawns = board.bitboards[Pieces::Pawn | color];

    if (color == Pieces::White) {
        attacks |= (pawns & ~left_file_mask) << 7;
        attacks |= (pawns & ~right_file_mask) << 9;
    } else {
        attacks |= (pawns & ~left_file_mask) >> 9;
        attacks |= (pawns & ~right_file_mask) >> 7;
    }

    return attacks;
}

void MoveGenerator::generateRookMoves(
    const Board& board, std::vector<Move>& moves, int color) const {

    int piece = Pieces::Rook | color;
    bitmask friends_free = ~(Pieces::isWhite(piece) ? board.white_pieces : board.black_pieces);

    for (int from : board.pieceLists[piece]) {
        bitmask targets = getRookAttack(board, magics, from) & friends_free;

        while (targets > 0) {
            int target = std::countr_zero(targets);
            moves.emplace_back(from, target);
            targets &= (targets - 1);
        }
    }
}

void MoveGenerator::generateBishopMoves(
    const Board& board, std::vector<Move>& moves, int color) const {

    int piece = Pieces::Bishop | color;
    bitmask friends_free = ~(Pieces::isWhite(piece) ? board.white_pieces : board.black_pieces);
    for (int from : board.pieceLists[piece]) {
        bitmask targets = getBishopAttack(board, magics, from) & friends_free;

        while (targets > 0) {
            int target = std::countr_zero(targets);
            moves.emplace_back(from, target);
            targets &= (targets - 1);
        }
    }
}

void MoveGenerator::generateQueenMoves(
    const Board& board, std::vector<Move>& moves, int color) const {

    int piece = Pieces::Queen | color;
    bitmask friends_free = ~(Pieces::isWhite(piece) ? board.white_pieces : board.black_pieces);
    for (int from : board.pieceLists[piece]) {

        bitmask targets1 = getRookAttack(board, magics, from);
        bitmask targets2 = getBishopAttack(board, magics, from);
        bitmask targets = (targets1 | targets2) & friends_free;

        while (targets > 0) {
            int target = std::countr_zero(targets);
            moves.emplace_back(from, target);
            targets &= (targets - 1);
        }
    }
}

void MoveGenerator::generatePawnMoves(
    const Board& board, std::vector<Move>& moves, int color) const {
    int piece = Pieces::Pawn | color;
    bool white_to_move = color == Pieces::White;

    int enemy_color = white_to_move ? Pieces::Black : Pieces::White;
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

bool MoveGenerator::isSquareAttacked(const Board& board, int square, int attacker_color) {

    if (precomputed.knightMoves[square] & board.bitboards[Pieces::Knight | attacker_color]) {
        return true;
    }

    // TODO: VERY IMPORTANT CHECK IF USING 1LL is ENOUGHT MAYBE IT SHOULD BE 1ULL
    bitmask pawn_attacks = getPawnsAttacks(board, attacker_color);
    if (pawn_attacks & (1LL << square)) {
        return true;
    }

    bitmask rook_rays = getRookAttack(board, magics, square);
    bitmask bishop_rays = getBishopAttack(board, magics, square);

    bitmask rooks_and_queens = board.bitboards[Pieces::Rook | attacker_color] |
                               board.bitboards[Pieces::Queen | attacker_color];

    bitmask bishops_and_queens = board.bitboards[Pieces::Bishop | attacker_color] |
                                 board.bitboards[Pieces::Queen | attacker_color];

    if ((rook_rays & rooks_and_queens) != 0) {
        return true;
    }

    if ((bishop_rays & bishops_and_queens) != 0) {
        return true;
    }

    if (precomputed.kingMoves[square] & board.bitboards[Pieces::King | attacker_color]) {
        return true;
    }

    return false;
}

// bitmask MoveGenerator::generateSideAttacks(const Board& board, int color) {
//     bitmask attacks = 0;

//     for (int from : board.pieceLists[Pieces::Rook | color]) {
//         attacks |= getRookAttack(board, magics, from);
//     }

//     for (int from : board.pieceLists[Pieces::Bishop | color]) {
//         attacks |= getBishopAttack(board, magics, from);
//     }

//     for (int from : board.pieceLists[Pieces::Queen | color]) {
//         attacks |= getRookAttack(board, magics, from);
//         attacks |= getBishopAttack(board, magics, from);
//     }

//     for (int from : board.pieceLists[Pieces::Knight | color]) {
//         attacks |= precomputed.knightMoves[from];
//     }

//     for (int from : board.pieceLists[Pieces::King | color]) {
//         attacks |= precomputed.kingMoves[from];
//     }

//     attacks |= getPawnsAttacks(board, color);

//     return attacks;
// }

// void MoveGenerator::generateAttacks(const Board& board) {
//     white_attacks = generateSideAttacks(board, Pieces::White);
//     black_attacks = generateSideAttacks(board, Pieces::Black);
// }

auto MoveGenerator::generateMoves(const Board& board) -> std::vector<Move> {
    std::vector<Move> moves;

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
    int response_color = board.white_to_move ? Pieces::Black : Pieces::White;

    std::vector<Move> moves = generateMoves(board);
    for (const Move& move : moves) {
        board.makeMove(move);
        int king_position = board.pieceLists[Pieces::King | color][0];

        // bitmask attacks = generateSideAttacks(board, response_color);
        if (!isSquareAttacked(board, king_position, response_color)) {
            legal_moves.push_back(move);
        }

        board.unmakeMove();
    }

    return legal_moves;
}
