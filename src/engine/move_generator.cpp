#include "move_generator.hpp"
#include "board.hpp"
#include "constants.hpp"
#include "magics.hpp"
#include "move.hpp"
#include "move_list.hpp"
#include "pieces.hpp"
#include "precomputed.hpp"
#include "utility.hpp"
#include <bit>

namespace MoveGenerator {
namespace {
    const Precomputed precomputed{};
    const Magics magics{};

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

    bool isSquareAttacked(const Board& board, int square, int attacker_color) {

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

    void generateKnightMoves(const Board& board, MoveList& moves, int color) {
        bitmask friendly_pieces =
            (color == Pieces::White) ? board.white_pieces : board.black_pieces;
        bitmask pieces = board.bitboards[Pieces::Knight | color];

        while (pieces) {
            int from = std::countr_zero(pieces);
            pieces &= (pieces - 1);

            bitmask mask = precomputed.knightMoves[from] & ~friendly_pieces;
            while (mask > 0) {
                int target = std::countr_zero(mask);
                moves.push({ from, target });
                mask &= (mask - 1);
            }
        }
    }

    void generateRookMoves(const Board& board, MoveList& moves, int color) {
        int piece = Pieces::Rook | color;
        bitmask friends_free = ~(Pieces::isWhite(piece) ? board.white_pieces : board.black_pieces);
        bitmask pieces = board.bitboards[piece];

        while (pieces) {
            int from = std::countr_zero(pieces);
            pieces &= (pieces - 1);

            bitmask targets = getRookAttack(board, magics, from) & friends_free;
            while (targets > 0) {
                int target = std::countr_zero(targets);
                moves.push({ from, target });
                targets &= (targets - 1);
            }
        }
    }

    void generateBishopMoves(const Board& board, MoveList& moves, int color) {
        int piece = Pieces::Bishop | color;
        bitmask friends_free = ~(Pieces::isWhite(piece) ? board.white_pieces : board.black_pieces);
        bitmask pieces = board.bitboards[piece];

        while (pieces) {
            int from = std::countr_zero(pieces);
            pieces &= (pieces - 1);

            bitmask targets = getBishopAttack(board, magics, from) & friends_free;
            while (targets > 0) {
                int target = std::countr_zero(targets);
                moves.push({ from, target });
                targets &= (targets - 1);
            }
        }
    }

    void generateQueenMoves(const Board& board, MoveList& moves, int color) {
        int piece = Pieces::Queen | color;
        bitmask friends_free = ~(Pieces::isWhite(piece) ? board.white_pieces : board.black_pieces);
        bitmask pieces = board.bitboards[piece];

        while (pieces) {
            int from = std::countr_zero(pieces);
            pieces &= (pieces - 1);

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

    void generatePawnMoves(const Board& board, MoveList& moves, int color) {
        int piece = Pieces::Pawn | color;
        bool white_to_move = color == Pieces::White;
        int dir = white_to_move ? 1 : -1;

        bitmask pawns = board.bitboards[piece];
        bitmask empty_squares = ~(board.white_pieces | board.black_pieces);
        bitmask enemies = white_to_move ? board.black_pieces : board.white_pieces;
        bitmask promotion_rank = white_to_move ? RANK_8 : RANK_1;

        if (board.enpassant_square != -1) {
            enemies |= setBit(board.enpassant_square);
        }

        bitmask single_pushes = 0;
        bitmask double_pushes = 0;
        bitmask left_attacks = 0;
        bitmask right_attacks = 0;

        auto movePawn = [&](int from, int to, MoveType type = MoveType::Normal) {
            if ((setBit(to) & promotion_rank) != 0) {
                moves.push({ from, to, MoveType::PromotionBishop });
                moves.push({ from, to, MoveType::PromotionKnight });
                moves.push({ from, to, MoveType::PromotionRook });
                moves.push({ from, to, MoveType::PromotionQueen });
            } else {
                moves.push({ from, to, type });
            }
        };

        auto processAttacks = [&](bitmask attacks, int offset) {
            while (attacks) {
                int to = std::countr_zero(attacks);
                int from = to - offset;

                if (to == board.enpassant_square) {
                    moves.push({ from, to, MoveType::EnPassant });
                } else {
                    movePawn(from, to);
                }

                attacks &= (attacks - 1);
            }
        };

        // NOLINTBEGIN
        if (color == Pieces::White) {
            single_pushes = (pawns << 8) & empty_squares;
            double_pushes = (single_pushes << 8) & empty_squares & RANK_4;

            left_attacks = (pawns << 7) & ~FILE_H & enemies;
            right_attacks = (pawns << 9) & ~FILE_A & enemies;

            processAttacks(left_attacks, 7);
            processAttacks(right_attacks, 9);
        } else {
            single_pushes = (pawns >> 8) & empty_squares;
            double_pushes = (single_pushes >> 8) & empty_squares & RANK_5;

            left_attacks = (pawns >> 9) & ~FILE_H & enemies;
            right_attacks = (pawns >> 7) & ~FILE_A & enemies;

            processAttacks(left_attacks, -9);
            processAttacks(right_attacks, -7);
        }

        while (single_pushes) {
            int target = std::countr_zero(single_pushes);
            int from = target - 8 * dir;
            movePawn(from, target);
            single_pushes &= (single_pushes - 1);
        }

        while (double_pushes) {
            int target = std::countr_zero(double_pushes);
            int from = target - 16 * dir;
            moves.push({ from, target, MoveType::DoublePush });
            double_pushes &= (double_pushes - 1);
        }
        // NOLINTEND
    }

    void generateKingMoves(const Board& board, MoveList& moves, int color) {

        int piece = Pieces::King | color;
        bitmask friends_free = ~(Pieces::isWhite(piece) ? board.white_pieces : board.black_pieces);

        bitmask pieces = board.bitboards[piece];
        int from = std::countr_zero(pieces);
        bitmask targets = precomputed.kingMoves[from] & friends_free;

        while (targets) {
            int target = std::countr_zero(targets);
            moves.push({ from, target });
            targets &= (targets - 1);
        }

        int enemy = Pieces::oppositeColor(color);
        auto checkSquares = [&](int sq1, int sq2) {
            if (board.squares[sq1] != Pieces::None || isSquareAttacked(board, sq1, enemy))
                return false;
            if (board.squares[sq2] != Pieces::None || isSquareAttacked(board, sq2, enemy))
                return false;
            return true;
        };

        using namespace Squares;

        if (color == Pieces::White) {
            if (from != E1 || isSquareAttacked(board, E1, enemy)) return;
            if (board.canCastle(Board::white_king_castle) && checkSquares(F1, G1)) {
                moves.push({ E1, G1, MoveType::Castling });
            }
            if (board.canCastle(Board::white_queen_castle) && checkSquares(D1, C1) &&
                board.squares[B1] == Pieces::None) {
                moves.push({ E1, C1, MoveType::Castling });
            }
        } else {
            if (from != E8 || isSquareAttacked(board, E8, enemy)) return;
            if (board.canCastle(Board::black_king_castle) && checkSquares(F8, G8)) {
                moves.push({ E8, G8, MoveType::Castling });
            }
            if (board.canCastle(Board::black_queen_castle) && checkSquares(D8, C8) &&
                board.squares[B8] == Pieces::None) {
                moves.push({ E8, C8, MoveType::Castling });
            }
        }
    }

} // namespace

auto generateMoves(const Board& board) -> MoveList {
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

auto generateLegalMoves(Board& board) -> MoveList {

    MoveList legal_moves;
    int color = board.white_to_move ? Pieces::White : Pieces::Black;
    int response_color = Pieces::oppositeColor(color);

    MoveList moves = generateMoves(board);
    for (const Move& move : moves) {
        board.makeMove(move);
        int king_position = std::countr_zero(board.bitboards[Pieces::King | color]);

        if (!isSquareAttacked(board, king_position, response_color)) {
            legal_moves.push(move);
        }

        board.unmakeMove();
    }

    return legal_moves;
}
} // namespace MoveGenerator