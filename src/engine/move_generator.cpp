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
#include <optional>

namespace MoveGenerator {
namespace {
    const Precomputed precomputed{};
    const Magics magics{};

    bitmask getRookAttack(const Board& board, const Magics& magics, int from) {
        bitmask all_pieces = board.white_pieces | board.black_pieces;
        bitmask blockers = all_pieces & magics.getRookMask(from);
        return magics.getRookAttacks(from, blockers);
    }

    bitmask getBishopAttack(const Board& board, const Magics& magics, int from) {
        bitmask all_pieces = board.white_pieces | board.black_pieces;
        bitmask blockers = all_pieces & magics.getBishopMask(from);
        return magics.getBishopAttacks(from, blockers);
    }

    bitmask getPawnsAttacks(const Board& board, Piece::Color color) {
        bitmask attacks = 0;
        bitmask pawns = board.bitboards[Piece(Piece::Pawn, color).value];

        if (color == Piece::White) {
            attacks |= (pawns & ~FILE_A) << 7;
            attacks |= (pawns & ~FILE_H) << 9;
        } else {
            attacks |= (pawns & ~FILE_A) >> 9;
            attacks |= (pawns & ~FILE_H) >> 7;
        }

        return attacks;
    }

    bitmask getRookAttack(bitmask all_pieces, const Magics& magics, int from) {
        // bitmask all_pieces = board.white_pieces | board.black_pieces;
        bitmask blockers = all_pieces & magics.getRookMask(from);
        return magics.getRookAttacks(from, blockers);
    }

    bitmask getBishopAttack(bitmask all_pieces, const Magics& magics, int from) {
        // bitmask all_pieces = board.white_pieces | board.black_pieces;
        bitmask blockers = all_pieces & magics.getBishopMask(from);
        return magics.getBishopAttacks(from, blockers);
    }

    bitmask getPawnsAttacks(bitmask pawns, Piece::Color color) {
        bitmask attacks = 0;
        // bitmask pawns = board.bitboards[Piece(Piece::Pawn, color).value];

        if (color == Piece::White) {
            attacks |= (pawns & ~FILE_A) << 7;
            attacks |= (pawns & ~FILE_H) << 9;
        } else {
            attacks |= (pawns & ~FILE_A) >> 9;
            attacks |= (pawns & ~FILE_H) >> 7;
        }

        return attacks;
    }

    bool isSquareAttackedBy(const Board& board, int square, Piece::Color attacker_color) {

        if (precomputed.knightMoves[square] &
            board.bitboards[Piece(Piece::Knight, attacker_color).value]) {
            return true;
        }

        bitmask pawn_attacks = getPawnsAttacks(board, attacker_color);
        if (pawn_attacks & setBit(square)) {
            return true;
        }

        bitmask rook_rays = getRookAttack(board, magics, square);
        bitmask bishop_rays = getBishopAttack(board, magics, square);

        bitmask rooks = board.bitboards[Piece(Piece::Rook, attacker_color).value];
        bitmask bishops = board.bitboards[Piece(Piece::Bishop, attacker_color).value];
        bitmask queens = board.bitboards[Piece(Piece::Queen, attacker_color).value];

        if ((rook_rays & (rooks | queens)) != 0) {
            return true;
        }

        if ((bishop_rays & (bishops | queens)) != 0) {
            return true;
        }

        if (precomputed.kingMoves[square] &
            board.bitboards[Piece(Piece::King, attacker_color).value]) {
            return true;
        }

        return false;
    }

    void generateKnightMoves(const Board& board, MoveList& moves, bool captures) {
        Piece::Color color = board.color_to_move;
        bitmask pieces = board.bitboards[Piece(Piece::Knight, color).value];
        bitmask friends = board.white_pieces;
        bitmask enemies = board.black_pieces;

        if (color == Piece::Black) {
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
        Piece::Color color = board.color_to_move;
        bitmask pieces = board.bitboards[Piece(Piece::Rook, color).value];
        bitmask friends = board.white_pieces;
        bitmask enemies = board.black_pieces;

        if (color == Piece::Black) {
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
        Piece::Color color = board.color_to_move;
        bitmask pieces = board.bitboards[Piece(Piece::Bishop, color).value];
        bitmask friends = board.white_pieces;
        bitmask enemies = board.black_pieces;

        if (color == Piece::Black) {
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
        Piece::Color color = board.color_to_move;
        bitmask pieces = board.bitboards[Piece(Piece::Queen, color).value];
        bitmask friends = board.white_pieces;
        bitmask enemies = board.black_pieces;

        if (color == Piece::Black) {
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
        Piece::Color color = board.color_to_move;
        Piece piece = Piece(Piece::Pawn, color);
        bitmask enemies = board.whiteToMove() ? board.black_pieces : board.white_pieces;
        bitmask pawns = board.bitboards[piece.value];
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
        Piece::Color color = board.color_to_move;
        Piece piece = Piece(Piece::Pawn, color);
        int dir = board.whiteToMove() ? 1 : -1;

        bitmask pawns = board.bitboards[piece.value];
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
        Piece::Color color = board.color_to_move;
        bitmask friends = board.white_pieces;
        bitmask enemies = board.black_pieces;

        if (color == Piece::Black) {
            std::swap(friends, enemies);
        }

        int from = std::countr_zero(board.bitboards[Piece(Piece::King, color).value]);

        bitmask targets = precomputed.kingMoves[from] & ~friends;
        if (captures) targets &= enemies;

        while (targets) {
            int target = readBit(targets);
            moves.push({ from, target });
        }

        if (captures) return;

        Piece::Color enemy = Piece::flipColor(color);
        auto checkSquares = [&](int sq1, int sq2) {
            if (board.squares[sq1] != Pieces::None || isSquareAttackedBy(board, sq1, enemy))
                return false;
            if (board.squares[sq2] != Pieces::None || isSquareAttackedBy(board, sq2, enemy))
                return false;
            return true;
        };

        using namespace Squares;

        if (color == Piece::White) {
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
        Piece::Color color = board.color_to_move;
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

bool isCheck(const Board& board, Piece::Color color) {
    int king_position = std::countr_zero(board.bitboards[Piece(Piece::King, color).value]);
    return isSquareAttackedBy(board, king_position, Piece::flipColor(color));
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

std::optional<int> squareAttackedFrom(const std::array<bitmask, MaxPiecesCount>& bitboards,
    bitmask all_pieces, int square, Piece::Color attacker_color) {

    bitmask pawns = bitboards[Piece(Piece::Pawn, attacker_color).value];
    bitmask pawn_attacks = getPawnsAttacks(pawns, attacker_color);
    if (pawn_attacks & setBit(square)) {
        return square;
    }

    if (precomputed.knightMoves[square] & bitboards[Piece(Piece::Knight, attacker_color).value]) {
        return square;
    }

    bitmask rook_rays = getRookAttack(all_pieces, magics, square);
    bitmask bishop_rays = getBishopAttack(all_pieces, magics, square);
    bitmask queen_rays = rook_rays | bishop_rays;

    bitmask rooks = bitboards[Piece(Piece::Rook, attacker_color).value];
    bitmask bishops = bitboards[Piece(Piece::Bishop, attacker_color).value];
    bitmask queens = bitboards[Piece(Piece::Queen, attacker_color).value];

    if ((bishop_rays & bishops) != 0) {
        return std::countr_zero(bishop_rays & bishops);
    }

    if ((rook_rays & rooks) != 0) {
        return std::countr_zero(rook_rays & rooks);
    }

    if ((queen_rays & queens) != 0) {
        return std::countr_zero(queen_rays & queens);
    }

    // if (precomputed.kingMoves[square] & board.bitboards[Piece(Piece::King,
    // attacker_color).value]) {
    //     return true;
    // }

    return std::nullopt;
}

} // namespace MoveGenerator