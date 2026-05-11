#include "move_generator.hpp"
#include "board.hpp"
#include "constants.hpp"
#include "magics.hpp"
#include "move.hpp"
#include "move_list.hpp"
#include "pieces.hpp"
#include "precomputed.hpp"
#include "utility.hpp"

using namespace Pieces;

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
        bitmask pawns = board.bitboards[makePiece(Pawn, color)];

        if (color == White) {
            attacks |= (pawns & ~FILE_A) << 7;
            attacks |= (pawns & ~FILE_H) << 9;
        } else {
            attacks |= (pawns & ~FILE_A) >> 9;
            attacks |= (pawns & ~FILE_H) >> 7;
        }

        return attacks;
    }

    bool isSquareAttackedBy(const Board& board, int square, int attacker_color) {

        if (precomputed.knightMoves[square] & board.bitboards[makePiece(Knight, attacker_color)]) {
            return true;
        }

        bitmask pawn_attacks = getPawnsAttacks(board, attacker_color);
        if (pawn_attacks & setBit(square)) {
            return true;
        }

        bitmask rook_rays = getRookAttack(board, magics, square);
        bitmask bishop_rays = getBishopAttack(board, magics, square);

        bitmask rooks = board.bitboards[makePiece(Rook, attacker_color)];
        bitmask bishops = board.bitboards[makePiece(Bishop, attacker_color)];
        bitmask queens = board.bitboards[makePiece(Queen, attacker_color)];

        if ((rook_rays & (rooks | queens)) != 0) {
            return true;
        }

        if ((bishop_rays & (bishops | queens)) != 0) {
            return true;
        }

        if (precomputed.kingMoves[square] & board.bitboards[makePiece(King, attacker_color)]) {
            return true;
        }

        return false;
    }

    void generateKnightMoves(const Board& board, MoveList& moves, bool captures) {
        int color = board.color_to_move;
        bitmask pieces = board.bitboards[makePiece(Knight, color)];
        bitmask friends = board.white_pieces;
        bitmask enemies = board.black_pieces;

        if (color == Black) {
            std::swap(friends, enemies);
        }

        while (pieces) {
            int from = readBit(pieces);
            bitmask mask = precomputed.knightMoves[from] & ~friends;
            if (captures) mask &= enemies;
            while (mask) {
                int target = readBit(mask);
                moves.push({ from, target });
            }
        }
    }

    void generateRookMoves(const Board& board, MoveList& moves, bool captures) {
        int color = board.color_to_move;
        bitmask pieces = board.bitboards[makePiece(Rook, color)];
        bitmask friends = board.white_pieces;
        bitmask enemies = board.black_pieces;

        if (color == Black) {
            std::swap(friends, enemies);
        }

        while (pieces) {
            int from = readBit(pieces);

            bitmask targets = getRookAttack(board, magics, from) & ~friends;
            if (captures) targets &= enemies;
            while (targets > 0) {
                int target = readBit(targets);
                moves.push({ from, target });
            }
        }
    }

    void generateBishopMoves(const Board& board, MoveList& moves, bool captures) {
        int color = board.color_to_move;
        bitmask pieces = board.bitboards[makePiece(Bishop, color)];
        bitmask friends = board.white_pieces;
        bitmask enemies = board.black_pieces;

        if (color == Black) {
            std::swap(friends, enemies);
        }

        while (pieces) {
            int from = readBit(pieces);

            bitmask targets = getBishopAttack(board, magics, from) & ~friends;
            if (captures) targets &= enemies;
            while (targets > 0) {
                int target = readBit(targets);
                moves.push({ from, target });
            }
        }
    }

    void generateQueenMoves(const Board& board, MoveList& moves, bool captures) {
        int color = board.color_to_move;
        bitmask pieces = board.bitboards[makePiece(Queen, color)];
        bitmask friends = board.white_pieces;
        bitmask enemies = board.black_pieces;

        if (color == Black) {
            std::swap(friends, enemies);
        }

        while (pieces) {
            int from = readBit(pieces);

            bitmask targets1 = getRookAttack(board, magics, from);
            bitmask targets2 = getBishopAttack(board, magics, from);
            bitmask targets = (targets1 | targets2) & ~friends;

            if (captures) targets &= enemies;

            while (targets > 0) {
                int target = readBit(targets);
                moves.push({ from, target });
            }
        }
    }

    void generatePawnCaptures(const Board& board, MoveList& moves) {
        int color = board.color_to_move;
        int piece = makePiece(Pawn, color);
        bitmask enemies = board.whiteToMove() ? board.black_pieces : board.white_pieces;
        bitmask pawns = board.bitboards[piece];
        bitmask promotion_rank = board.whiteToMove() ? RANK_8 : RANK_1;

        if (board.enpassant_square != -1) {
            enemies |= setBit(board.enpassant_square);
        }

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
                int to = readBit(attacks);
                int from = to - offset;

                if (to == board.enpassant_square) {
                    moves.push({ from, to, MoveType::EnPassant });
                } else {
                    movePawn(from, to);
                }
            }
        };

        // NOLINTBEGIN
        if (board.whiteToMove()) {
            left_attacks = (pawns << 7) & ~FILE_H & enemies;
            right_attacks = (pawns << 9) & ~FILE_A & enemies;

            processAttacks(left_attacks, 7);
            processAttacks(right_attacks, 9);
        } else {
            left_attacks = (pawns >> 9) & ~FILE_H & enemies;
            right_attacks = (pawns >> 7) & ~FILE_A & enemies;

            processAttacks(left_attacks, -9);
            processAttacks(right_attacks, -7);
        }
        // NOLINTEND
    }

    void generatePawnMoves(const Board& board, MoveList& moves) {
        int color = board.color_to_move;
        int piece = makePiece(Pawn, color);
        int dir = board.whiteToMove() ? 1 : -1;

        bitmask pawns = board.bitboards[piece];
        bitmask empty_squares = ~(board.white_pieces | board.black_pieces);
        bitmask promotion_rank = board.whiteToMove() ? RANK_8 : RANK_1;

        bitmask single_pushes = 0;
        bitmask double_pushes = 0;

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

        // NOLINTBEGIN
        if (board.whiteToMove()) {
            single_pushes = (pawns << 8) & empty_squares;
            double_pushes = (single_pushes << 8) & empty_squares & RANK_4;
        } else {
            single_pushes = (pawns >> 8) & empty_squares;
            double_pushes = (single_pushes >> 8) & empty_squares & RANK_5;
        }

        while (single_pushes) {
            int target = readBit(single_pushes);
            int from = target - 8 * dir;
            movePawn(from, target);
        }

        while (double_pushes) {
            int target = readBit(double_pushes);
            int from = target - 16 * dir;
            moves.push({ from, target, MoveType::DoublePush });
        }
        // NOLINTEND

        generatePawnCaptures(board, moves);
    }

    void generateKingMoves(const Board& board, MoveList& moves, bool captures) {
        int color = board.color_to_move;
        bitmask friends = board.white_pieces;
        bitmask enemies = board.black_pieces;

        if (color == Pieces::Black) {
            std::swap(friends, enemies);
        }

        int from = std::countr_zero(board.bitboards[makePiece(King, color)]);

        bitmask targets = precomputed.kingMoves[from] & ~friends;
        if (captures) targets &= enemies;

        while (targets) {
            int target = readBit(targets);
            moves.push({ from, target });
        }

        if (captures) return;

        int enemy = flipColor(color);
        auto checkSquares = [&](int sq1, int sq2) {
            if (board.squares[sq1] != None || isSquareAttackedBy(board, sq1, enemy)) return false;
            if (board.squares[sq2] != None || isSquareAttackedBy(board, sq2, enemy)) return false;
            return true;
        };

        using namespace Squares;

        if (color == White) {
            if (from != E1 || isSquareAttackedBy(board, E1, enemy)) return;
            if (board.canCastle(Board::white_king_castle) && checkSquares(F1, G1)) {
                moves.push({ E1, G1, MoveType::Castling });
            }
            if (board.canCastle(Board::white_queen_castle) && checkSquares(D1, C1) &&
                board.squares[B1] == Pieces::None) {
                moves.push({ E1, C1, MoveType::Castling });
            }
        } else {
            if (from != E8 || isSquareAttackedBy(board, E8, enemy)) return;
            if (board.canCastle(Board::black_king_castle) && checkSquares(F8, G8)) {
                moves.push({ E8, G8, MoveType::Castling });
            }
            if (board.canCastle(Board::black_queen_castle) && checkSquares(D8, C8) &&
                board.squares[B8] == Pieces::None) {
                moves.push({ E8, C8, MoveType::Castling });
            }
        }
    }

    auto filterLegalMoves(Board& board, const MoveList& moves) -> MoveList {
        int color = board.color_to_move;
        MoveList legal_moves;

        for (const Move& move : moves) {
            board.makeMove(move);
            if (!isCheck(board, color)) {
                legal_moves.push(move);
            }
            board.unmakeMove();
        }

        return legal_moves;
    }

} // namespace

bool isCheck(const Board& board, int color) {
    int king_position = std::countr_zero(board.bitboards[makePiece(King, color)]);
    return isSquareAttackedBy(board, king_position, flipColor(color));
}

auto generateMoves(const Board& board) -> MoveList {
    MoveList moves;

    generateKnightMoves(board, moves, false);
    generateRookMoves(board, moves, false);
    generateBishopMoves(board, moves, false);
    generateQueenMoves(board, moves, false);
    generatePawnMoves(board, moves);
    generateKingMoves(board, moves, false);

    return moves;
}

auto generateCaptures(Board& board) -> MoveList {
    MoveList captures;

    generateKnightMoves(board, captures, true);
    generateRookMoves(board, captures, true);
    generateBishopMoves(board, captures, true);
    generateQueenMoves(board, captures, true);
    generatePawnCaptures(board, captures);
    generateKingMoves(board, captures, true);

    return captures;
}

auto generateLegalMoves(Board& board) -> MoveList {
    MoveList moves = generateMoves(board);
    return filterLegalMoves(board, moves);
}

auto generateLegalCaptures(Board& board) -> MoveList {
    return filterLegalMoves(board, generateCaptures(board));
}

} // namespace MoveGenerator