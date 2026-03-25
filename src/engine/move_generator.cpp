#include "move_generator.hpp"
#include "board.hpp"
#include "constants.hpp"
#include "magics.hpp"
#include "move.hpp"
#include "move_list.hpp"
#include "pieces.hpp"
#include "precomputed.hpp"
#include <bit>

bitmask setBit(int square) {
    return 1ULL << square;
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
    bitmask pawns = board.bitboards[Pieces::Pawn | color];

    if (color == Pieces::White) {
        attacks |= (pawns & ~FILE_A) << 7;
        attacks |= (pawns & ~FILE_H) << 9;
    } else {
        attacks |= (pawns & ~FILE_A) >> 9;
        attacks |= (pawns & ~FILE_H) >> 7;
    }

    return attacks;
}

void MoveGenerator::generateKnightMoves(const Board& board, MoveList& moves, int color) const {
    int piece = Pieces::Knight | color;
    bitmask friendly_pieces = color == Pieces::White ? board.white_pieces : board.black_pieces;
    for (int from : board.pieceLists[piece]) {
        bitmask mask = precomputed.knightMoves[from] & ~friendly_pieces;

        while (mask > 0) {
            int target = std::countr_zero(mask);
            moves.push({ from, target });
            mask &= (mask - 1);
        }
    }
}

void MoveGenerator::generateRookMoves(const Board& board, MoveList& moves, int color) const {

    int piece = Pieces::Rook | color;
    bitmask friends_free = ~(Pieces::isWhite(piece) ? board.white_pieces : board.black_pieces);

    for (int from : board.pieceLists[piece]) {
        bitmask targets = getRookAttack(board, magics, from) & friends_free;

        while (targets > 0) {
            int target = std::countr_zero(targets);
            moves.push({ from, target });
            targets &= (targets - 1);
        }
    }
}

void MoveGenerator::generateBishopMoves(const Board& board, MoveList& moves, int color) const {

    int piece = Pieces::Bishop | color;
    bitmask friends_free = ~(Pieces::isWhite(piece) ? board.white_pieces : board.black_pieces);
    for (int from : board.pieceLists[piece]) {
        bitmask targets = getBishopAttack(board, magics, from) & friends_free;

        while (targets > 0) {
            int target = std::countr_zero(targets);
            moves.push({ from, target });
            targets &= (targets - 1);
        }
    }
}

void MoveGenerator::generateQueenMoves(const Board& board, MoveList& moves, int color) const {

    int piece = Pieces::Queen | color;
    bitmask friends_free = ~(Pieces::isWhite(piece) ? board.white_pieces : board.black_pieces);
    for (int from : board.pieceLists[piece]) {

        bitmask targets1 = getRookAttack(board, magics, from);
        bitmask targets2 = getBishopAttack(board, magics, from);
        bitmask targets = (targets1 | targets2) & friends_free;

        while (targets > 0) {
            int target = std::countr_zero(targets);
            moves.push({ from, target });
            targets &= (targets - 1);
        }
    }
}

void MoveGenerator::generatePawnMoves(const Board& board, MoveList& moves, int color) const {
    int piece = Pieces::Pawn | color;
    bool white_to_move = color == Pieces::White;

    int enemy_color = white_to_move ? Pieces::Black : Pieces::White;
    int dir = white_to_move ? 1 : -1;

    bitmask pawns = board.bitboards[piece];
    bitmask empty_squares = ~(board.white_pieces | board.black_pieces);

    bitmask enemies = white_to_move ? board.black_pieces : board.white_pieces;
    if (board.enpassant_square != -1) {
        enemies |= setBit(board.enpassant_square);
    }

    bitmask promotion_pushes = 0;
    bitmask normal_pushes = 0;
    bitmask double_pushes = 0;
    bitmask left_attacks = 0;
    bitmask right_attacks = 0;

    auto processAttacks = [&](bitmask attacks, int offset) {
        while (attacks) {
            int to = std::countr_zero(attacks);
            int from = to - offset;

            if (to == board.enpassant_square) {
                moves.push({ from, to, MoveType::EnPassant });
            } else {
                moves.push({ from, to });
            }

            attacks &= (attacks - 1);
        }
    };

    if (color == Pieces::White) {
        bitmask single_pushes = (pawns << 8) & empty_squares;

        promotion_pushes = single_pushes & RANK_8;
        normal_pushes = single_pushes & ~RANK_8;

        double_pushes = (single_pushes << 8) & empty_squares & RANK_4;

        left_attacks = (pawns << 7) & ~FILE_H & enemies;
        right_attacks = (pawns << 9) & ~FILE_A & enemies;

        processAttacks(left_attacks, 7);
        processAttacks(right_attacks, 9);
    } else {
        bitmask single_pushes = (pawns >> 8) & empty_squares;

        promotion_pushes = single_pushes & RANK_1;
        normal_pushes = single_pushes & ~RANK_1;

        double_pushes = (single_pushes >> 8) & empty_squares & RANK_5;

        left_attacks = (pawns >> 9) & ~FILE_H & enemies;
        right_attacks = (pawns >> 7) & ~FILE_A & enemies;

        processAttacks(left_attacks, -9);
        processAttacks(right_attacks, -7);
    }

    while (normal_pushes) {
        int target = std::countr_zero(normal_pushes);
        int from = target - 8 * dir;
        moves.push({ from, target });
        normal_pushes &= (normal_pushes - 1);
    }

    while (promotion_pushes) {
        int to = std::countr_zero(promotion_pushes);
        int from = to - 8 * dir;
        moves.push({ from, to, MoveType::PromotionBishop });
        moves.push({ from, to, MoveType::PromotionKnight });
        moves.push({ from, to, MoveType::PromotionRook });
        moves.push({ from, to, MoveType::PromotionQueen });
        promotion_pushes &= (promotion_pushes - 1);
    }

    while (double_pushes) {
        int target = std::countr_zero(double_pushes);
        int from = target - 16 * dir;
        moves.push({ from, target, MoveType::DoublePush });
        double_pushes &= (double_pushes - 1);
    }
}

void MoveGenerator::generateKingMoves(const Board& board, MoveList& moves, int color) const {

    int piece = Pieces::King | color;
    bitmask friendly_pieces = color == Pieces::White ? board.white_pieces : board.black_pieces;

    for (int from : board.pieceLists[piece]) {
        bitmask mask = precomputed.kingMoves[from] & ~friendly_pieces;

        while (mask > 0) {
            int target = std::countr_zero(mask);
            moves.push({ from, target });
            mask &= (mask - 1);
        }
    }

    // TODO: implement castling
}

bool MoveGenerator::isSquareAttacked(const Board& board, int square, int attacker_color) {

    if (precomputed.knightMoves[square] & board.bitboards[Pieces::Knight | attacker_color]) {
        return true;
    }

    bitmask pawn_attacks = getPawnsAttacks(board, attacker_color);
    if (pawn_attacks & setBit(square)) {
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

auto MoveGenerator::generateMoves(const Board& board) -> MoveList {
    MoveList moves;

    int color = board.white_to_move ? Pieces::White : Pieces::Black;
    generateKnightMoves(board, moves, color);
    generateRookMoves(board, moves, color);
    generateBishopMoves(board, moves, color);
    generateQueenMoves(board, moves, color);
    generatePawnMoves(board, moves, color);
    generateKingMoves(board, moves, color);

    return moves;
}

auto MoveGenerator::generateLegalMoves(Board& board) -> MoveList {

    MoveList legal_moves;
    int color = board.white_to_move ? Pieces::White : Pieces::Black;
    int response_color = board.white_to_move ? Pieces::Black : Pieces::White;

    MoveList moves = generateMoves(board);
    for (const Move& move : moves) {
        board.makeMove(move);
        int king_position = board.pieceLists[Pieces::King | color][0];

        if (!isSquareAttacked(board, king_position, response_color)) {
            legal_moves.push(move);
        }

        board.unmakeMove();
    }

    return legal_moves;
}
