#include "move_generator.hpp"
#include "bitboard.hpp"
#include "board.hpp"
#include "constants.hpp"
#include "magics.hpp"
#include "move.hpp"
#include "move_list.hpp"
#include "pieces.hpp"
#include "precomputed.hpp"
#include "square.hpp"
#include "utility.hpp"
#include <bit>
#include <optional>

namespace MoveGenerator {
namespace {
    const Precomputed precomputed{};
    const Magics magics{};

    Bitboard getRookAttack(Bitboard all_pieces, const Magics& magics, Square from) {
        Bitboard blockers = all_pieces & magics.getRookMask(from);
        return magics.getRookAttacks(from, blockers.value);
    }

    Bitboard getBishopAttack(Bitboard all_pieces, const Magics& magics, Square from) {
        Bitboard blockers = all_pieces & magics.getBishopMask(from);
        return magics.getBishopAttacks(from, blockers.value);
    }

    Bitboard getPawnsAttacks(Bitboard pawns, Piece::Color color) {
        Bitboard attacks;

        if (color == Piece::White) {
            attacks |= Bitboard{ (pawns & ~FILE_A).value << 7 };
            attacks |= Bitboard{ (pawns & ~FILE_H).value << 9 };
        } else {
            attacks |= Bitboard{ (pawns & ~FILE_A).value >> 9 };
            attacks |= Bitboard{ (pawns & ~FILE_H).value >> 7 };
        }

        return attacks;
    }

    Bitboard getRookAttack(const Board& board, const Magics& magics, Square from) {
        Bitboard all_pieces = board.white_pieces | board.black_pieces;
        // Bitboard blockers = all_pieces & magics.getRookMask(from);
        // return magics.getRookAttacks(from, blockers.value);
        return getRookAttack(all_pieces, magics, from);
    }

    Bitboard getBishopAttack(const Board& board, const Magics& magics, Square from) {
        Bitboard all_pieces = board.white_pieces | board.black_pieces;
        // Bitboard blockers = all_pieces & magics.getBishopMask(from);
        // return magics.getBishopAttacks(from, blockers.value);
        return getBishopAttack(all_pieces, magics, from);
    }

    Bitboard getPawnsAttacks(const Board& board, Piece::Color color) {
        // Bitboard attacks;
        Bitboard pawns = board.bitboards[Piece(Piece::Pawn, color).value];

        // if (color == Piece::White) {
        //     attacks |= Bitboard{ (pawns & ~FILE_A).value << 7 };
        //     attacks |= Bitboard{ (pawns & ~FILE_H).value << 9 };
        // } else {
        //     attacks |= Bitboard{ (pawns & ~FILE_A).value >> 9 };
        //     attacks |= Bitboard{ (pawns & ~FILE_H).value >> 7 };
        // }

        // return attacks;
        return getPawnsAttacks(pawns, color);
    }

    bool isSquareAttackedBy(const Board& board, Square square, Piece::Color attacker_color) {

        if (precomputed.knightMoves[square] &
            board.bitboards[Piece(Piece::Knight, attacker_color).value]) {
            return true;
        }

        Bitboard pawn_attacks = getPawnsAttacks(board, attacker_color);
        if (pawn_attacks & Bitboard(square)) {
            return true;
        }

        Bitboard rook_rays = getRookAttack(board, magics, square);
        Bitboard bishop_rays = getBishopAttack(board, magics, square);

        Bitboard rooks = board.bitboards[Piece(Piece::Rook, attacker_color).value];
        Bitboard bishops = board.bitboards[Piece(Piece::Bishop, attacker_color).value];
        Bitboard queens = board.bitboards[Piece(Piece::Queen, attacker_color).value];

        if (rook_rays & (rooks | queens)) {
            return true;
        }

        if (bishop_rays & (bishops | queens)) {
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
        Bitboard pieces = board.bitboards[Piece(Piece::Knight, color).value];
        Bitboard friends = board.white_pieces;
        Bitboard enemies = board.black_pieces;

        if (color == Piece::Black) {
            std::swap(friends, enemies);
        }

        while (pieces) {
            int from = pieces.readBit();
            Bitboard mask = precomputed.knightMoves[from] & ~friends;
            if (captures) mask &= enemies;
            while (mask) {
                int target = mask.readBit();
                moves.push({ Square(from), Square(target) });
            }
        }
    }

    void generateRookMoves(const Board& board, MoveList& moves, bool captures) {
        Piece::Color color = board.color_to_move;
        Bitboard pieces = board.bitboards[Piece(Piece::Rook, color).value];
        Bitboard friends = board.white_pieces;
        Bitboard enemies = board.black_pieces;

        if (color == Piece::Black) {
            std::swap(friends, enemies);
        }

        while (pieces) {
            Square from{ pieces.readBit() };

            Bitboard targets = getRookAttack(board, magics, from) & ~friends;
            if (captures) targets &= enemies;
            while (targets) {
                Square target{ targets.readBit() };
                moves.push({ from, target });
            }
        }
    }

    void generateBishopMoves(const Board& board, MoveList& moves, bool captures) {
        Piece::Color color = board.color_to_move;
        Bitboard pieces = board.bitboards[Piece(Piece::Bishop, color).value];
        Bitboard friends = board.white_pieces;
        Bitboard enemies = board.black_pieces;

        if (color == Piece::Black) {
            std::swap(friends, enemies);
        }

        while (pieces) {
            Square from{ pieces.readBit() };

            Bitboard targets = getBishopAttack(board, magics, from) & ~friends;
            if (captures) targets &= enemies;
            while (targets) {
                Square target{ targets.readBit() };
                moves.push({ from, target });
            }
        }
    }

    void generateQueenMoves(const Board& board, MoveList& moves, bool captures) {
        Piece::Color color = board.color_to_move;
        Bitboard pieces = board.bitboards[Piece(Piece::Queen, color).value];
        Bitboard friends = board.white_pieces;
        Bitboard enemies = board.black_pieces;

        if (color == Piece::Black) {
            std::swap(friends, enemies);
        }

        while (pieces) {
            Square from{ pieces.readBit() };

            Bitboard targets1 = getRookAttack(board, magics, from);
            Bitboard targets2 = getBishopAttack(board, magics, from);
            Bitboard targets = (targets1 | targets2) & ~friends;

            if (captures) targets &= enemies;

            while (targets) {
                Square target{ targets.readBit() };
                moves.push({ from, target });
            }
        }
    }

    void generatePawnCaptures(const Board& board, MoveList& moves) {
        Piece::Color color = board.color_to_move;
        Piece piece = Piece(Piece::Pawn, color);
        Bitboard enemies = board.whiteToMove() ? board.black_pieces : board.white_pieces;
        Bitboard pawns = board.bitboards[piece.value];
        Bitboard promotion_rank = board.whiteToMove() ? RANK_8 : RANK_1;

        if (board.enpassant_square != Squares::None) {
            enemies |= Bitboard(board.enpassant_square);
        }

        Bitboard left_attacks;
        Bitboard right_attacks;

        auto movePawn = [&](Square from, Square to, MoveType type = MoveType::Normal) {
            if (Bitboard(to) & promotion_rank) {
                moves.push({ from, to, MoveType::PromotionBishop });
                moves.push({ from, to, MoveType::PromotionKnight });
                moves.push({ from, to, MoveType::PromotionRook });
                moves.push({ from, to, MoveType::PromotionQueen });
            } else {
                moves.push({ from, to, type });
            }
        };

        auto processAttacks = [&](Bitboard attacks, int offset) {
            while (attacks) {
                Square to{ attacks.readBit() };
                Square from{ to - offset };

                if (to == board.enpassant_square) {
                    moves.push({ from, to, MoveType::EnPassant });
                } else {
                    movePawn(from, to);
                }
            }
        };

        // NOLINTBEGIN
        if (board.whiteToMove()) {
            left_attacks = Bitboard(pawns.value << 7) & ~FILE_H & enemies;
            right_attacks = Bitboard(pawns.value << 9) & ~FILE_A & enemies;

            processAttacks(left_attacks, 7);
            processAttacks(right_attacks, 9);
        } else {
            left_attacks = Bitboard(pawns.value >> 9) & ~FILE_H & enemies;
            right_attacks = Bitboard(pawns.value >> 7) & ~FILE_A & enemies;

            processAttacks(left_attacks, -9);
            processAttacks(right_attacks, -7);
        }
        // NOLINTEND
    }

    void generatePawnMoves(const Board& board, MoveList& moves) {
        Piece::Color color = board.color_to_move;
        Piece piece = Piece(Piece::Pawn, color);
        int dir = board.whiteToMove() ? 1 : -1;

        Bitboard pawns = board.bitboards[piece.value];
        Bitboard empty_squares = ~(board.white_pieces | board.black_pieces);
        Bitboard promotion_rank = board.whiteToMove() ? RANK_8 : RANK_1;

        Bitboard single_pushes;
        Bitboard double_pushes;

        auto movePawn = [&](Square from, Square to, MoveType type = MoveType::Normal) {
            if (Bitboard(to) & promotion_rank) {
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
            single_pushes = Bitboard(pawns.value << 8) & empty_squares;
            double_pushes = Bitboard(single_pushes.value << 8) & empty_squares & RANK_4;
        } else {
            single_pushes = Bitboard(pawns.value >> 8) & empty_squares;
            double_pushes = Bitboard(single_pushes.value >> 8) & empty_squares & RANK_5;
        }

        while (single_pushes) {
            int target = single_pushes.readBit();
            int from = target - 8 * dir;
            movePawn(Square(from), Square(target));
        }

        while (double_pushes) {
            int target = double_pushes.readBit();
            int from = target - 16 * dir;
            moves.push({ Square(from), Square(target), MoveType::DoublePush });
        }
        // NOLINTEND

        generatePawnCaptures(board, moves);
    }

    void generateKingMoves(const Board& board, MoveList& moves, bool captures) {
        Piece::Color color = board.color_to_move;
        Bitboard friends = board.white_pieces;
        Bitboard enemies = board.black_pieces;

        if (color == Piece::Black) {
            std::swap(friends, enemies);
        }

        Square from{ std::countr_zero(board.bitboards[Piece(Piece::King, color).value].value) };

        Bitboard targets = precomputed.kingMoves[from] & ~friends;
        if (captures) targets &= enemies;

        while (targets) {
            auto target = Square(targets.readBit());
            moves.push({ from, target });
        }

        if (captures) return;

        Piece::Color enemy = Piece::flipColor(color);
        auto checkSquares = [&](Square sq1, Square sq2) {
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
    Square king_position{ std::countr_zero(
        board.bitboards[Piece(Piece::King, color).value].value) };
    return isSquareAttackedBy(board, Square(king_position), Piece::flipColor(color));
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

// std::optional<Square> squareAttackedFrom(const std::array<bitmask, MaxPiecesCount>& bitboards,
//     bitmask all_pieces, Square square, Piece::Color attacker_color) {

//     bitmask pawns = bitboards[Piece(Piece::Pawn, attacker_color).value];
//     bitmask pawn_attacks = getPawnsAttacks(pawns, attacker_color);
//     if (pawn_attacks & setBit(square)) {
//         return square;
//     }

//     if (precomputed.knightMoves[square] & bitboards[Piece(Piece::Knight, attacker_color).value])
//     {
//         return square;
//     }

//     bitmask rook_rays = getRookAttack(all_pieces, magics, square);
//     bitmask bishop_rays = getBishopAttack(all_pieces, magics, square);
//     bitmask queen_rays = rook_rays | bishop_rays;

//     bitmask rooks = bitboards[Piece(Piece::Rook, attacker_color).value];
//     bitmask bishops = bitboards[Piece(Piece::Bishop, attacker_color).value];
//     bitmask queens = bitboards[Piece(Piece::Queen, attacker_color).value];

//     if ((bishop_rays & bishops) != 0) {
//         return Square(std::countr_zero(bishop_rays & bishops));
//     }

//     if ((rook_rays & rooks) != 0) {
//         return Square(std::countr_zero(rook_rays & rooks));
//     }

//     if ((queen_rays & queens) != 0) {
//         return Square(std::countr_zero(queen_rays & queens));
//     }

//     // if (precomputed.kingMoves[square] & board.bitboards[Piece(Piece::King,
//     // attacker_color).value]) {
//     //     return true;
//     // }

//     return std::nullopt;
// }

} // namespace MoveGenerator