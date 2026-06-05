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

namespace MoveGenerator {
namespace {
    const Precomputed precomputed{};
    const Magics magics{};

    Bitboard getRookAttack(Bitboard all_pieces, Square from) {
        Bitboard blockers = all_pieces & magics.getRookMask(from);
        return magics.getRookAttacks(from, blockers.value);
    }

    Bitboard getBishopAttack(Bitboard all_pieces, Square from) {
        Bitboard blockers = all_pieces & magics.getBishopMask(from);
        return magics.getBishopAttacks(from, blockers.value);
    }

    Bitboard getPawnsAttacks(Bitboard pawns, Piece::Color color) {
        Bitboard attacks;

        if (color == Piece::White) {
            attacks |= pawns.shift_north_west();
            attacks |= pawns.shift_north_east();
        } else {
            attacks |= pawns.shift_south_west();
            attacks |= pawns.shift_south_east();
        }

        return attacks;
    }

    Bitboard getRookAttack(const Board& board, Square from) {
        return getRookAttack(board.pieces(), from);
    }

    Bitboard getBishopAttack(const Board& board, Square from) {
        return getBishopAttack(board.pieces(), from);
    }

    Bitboard getPawnsAttacks(const Board& board, Piece::Color color) {
        Bitboard pawns = board.pieces(Piece::Pawn, color);
        return getPawnsAttacks(pawns, color);
    }

    bool isSquareAttackedBy(const Board& board, Square square, Piece::Color attacker_color) {
        using enum Piece::Type;

        if (precomputed.knightMoves[square] & board.pieces(Knight, attacker_color)) {
            return true;
        }

        if (getPawnsAttacks(board, attacker_color).test(square)) {
            return true;
        }

        Bitboard rook_rays = getRookAttack(board, square);
        Bitboard bishop_rays = getBishopAttack(board, square);

        Bitboard rooks = board.pieces(Rook, attacker_color);
        Bitboard bishops = board.pieces(Bishop, attacker_color);
        Bitboard queens = board.pieces(Queen, attacker_color);

        if (rook_rays & (rooks | queens)) {
            return true;
        }

        if (bishop_rays & (bishops | queens)) {
            return true;
        }

        if (precomputed.kingMoves[square] & board.pieces(King, attacker_color)) {
            return true;
        }

        return false;
    }

    void generateKnightMoves(const Board& board, MoveList& moves, bool captures) {
        Piece::Color color = board.color_to_move;
        Bitboard pieces = board.pieces(Piece::Knight, color);
        Bitboard friends = board.pieces(color);
        Bitboard enemies = board.pieces(Piece::flipColor(color));

        while (pieces) {
            Square from = pieces.readBit();
            Bitboard mask = precomputed.knightMoves[from] & ~friends;
            if (captures) mask &= enemies;
            while (mask) {
                Square target = mask.readBit();
                moves.push({ from, target });
            }
        }
    }

    void generateRookMoves(const Board& board, MoveList& moves, bool captures) {
        Piece::Color color = board.color_to_move;
        Bitboard pieces = board.pieces(Piece::Rook, color);
        Bitboard friends = board.pieces(color);
        Bitboard enemies = board.pieces(Piece::flipColor(color));

        while (pieces) {
            Square from = pieces.readBit();

            Bitboard targets = getRookAttack(board, from) & ~friends;
            if (captures) targets &= enemies;
            while (targets) {
                Square target = targets.readBit();
                moves.push({ from, target });
            }
        }
    }

    void generateBishopMoves(const Board& board, MoveList& moves, bool captures) {
        Piece::Color color = board.color_to_move;
        Bitboard pieces = board.pieces(Piece::Bishop, color);
        Bitboard friends = board.pieces(color);
        Bitboard enemies = board.pieces(Piece::flipColor(color));

        while (pieces) {
            Square from = pieces.readBit();

            Bitboard targets = getBishopAttack(board, from) & ~friends;
            if (captures) targets &= enemies;
            while (targets) {
                Square target = targets.readBit();
                moves.push({ from, target });
            }
        }
    }

    void generateQueenMoves(const Board& board, MoveList& moves, bool captures) {
        Piece::Color color = board.color_to_move;
        Bitboard pieces = board.pieces(Piece::Queen, color);
        Bitboard friends = board.pieces(color);
        Bitboard enemies = board.pieces(Piece::flipColor(color));

        while (pieces) {
            Square from = pieces.readBit();

            Bitboard targets1 = getRookAttack(board, from);
            Bitboard targets2 = getBishopAttack(board, from);
            Bitboard targets = (targets1 | targets2) & ~friends;

            if (captures) targets &= enemies;

            while (targets) {
                Square target = targets.readBit();
                moves.push({ from, target });
            }
        }
    }

    void addPawnMove(MoveList& moves, int promotion_rank, Square from, Square to) {
        if (to.rank() == promotion_rank) {
            moves.push({ from, to, MoveType::PromotionBishop });
            moves.push({ from, to, MoveType::PromotionKnight });
            moves.push({ from, to, MoveType::PromotionRook });
            moves.push({ from, to, MoveType::PromotionQueen });
        } else {
            moves.push({ from, to, MoveType::Normal });
        }
    }

    void generatePawnCaptures(const Board& board, MoveList& moves) {
        Bitboard enemies = board.pieces(Piece::flipColor(board.color_to_move));
        Bitboard pawns = board.pieces(Piece::Pawn, board.color_to_move);
        int promotion_rank = board.whiteToMove() ? 7 : 0;

        if (board.enpassant_square != Squares::None) {
            enemies |= Bitboard(board.enpassant_square);
        }

        Bitboard left_attacks;
        Bitboard right_attacks;

        auto processAttacks = [&](Bitboard attacks, int offset) {
            while (attacks) {
                Square to = attacks.readBit();
                Square from = to - offset;

                if (to == board.enpassant_square) {
                    moves.push({ from, to, MoveType::EnPassant });
                } else {
                    addPawnMove(moves, promotion_rank, from, to);
                }
            }
        };

        if (board.whiteToMove()) {
            left_attacks = pawns.shift_north_west() & enemies;
            right_attacks = pawns.shift_north_east() & enemies;

            processAttacks(left_attacks, 7);
            processAttacks(right_attacks, 9);
        } else {
            left_attacks = pawns.shift_south_west() & enemies;
            right_attacks = pawns.shift_south_east() & enemies;

            processAttacks(left_attacks, -9);
            processAttacks(right_attacks, -7);
        }
    }

    void generatePawnMoves(const Board& board, MoveList& moves) {
        int dir = board.whiteToMove() ? 1 : -1;
        Bitboard pawns = board.pieces(Piece::Pawn, board.color_to_move);
        Bitboard empty_squares = ~board.pieces();
        int promotion_rank = board.whiteToMove() ? 7 : 0;

        Bitboard single_pushes;
        Bitboard double_pushes;

        if (board.whiteToMove()) {
            single_pushes = pawns.shift_north() & empty_squares;
            double_pushes = single_pushes.shift_north() & empty_squares & RANK_4;
        } else {
            single_pushes = pawns.shift_south() & empty_squares;
            double_pushes = single_pushes.shift_south() & empty_squares & RANK_5;
        }

        while (single_pushes) {
            Square target = single_pushes.readBit();
            Square from = target - (8 * dir);
            addPawnMove(moves, promotion_rank, from, target);
        }

        while (double_pushes) {
            Square target = double_pushes.readBit();
            Square from = target - (16 * dir);
            moves.push({ from, target, MoveType::DoublePush });
        }

        generatePawnCaptures(board, moves);
    }

    void generateKingMoves(const Board& board, MoveList& moves, bool captures) {
        Piece::Color color = board.color_to_move;
        Bitboard friends = board.pieces(color);
        Bitboard enemies = board.pieces(Piece::flipColor(color));

        Bitboard king_bitboard = board.pieces(Piece::King, color);
        Square from = king_bitboard.readBit();

        Bitboard targets = precomputed.kingMoves[from] & ~friends;
        if (captures) targets &= enemies;

        while (targets) {
            Square target = targets.readBit();
            moves.push({ from, target });
        }

        if (captures) return;

        Piece::Color enemy = Piece::flipColor(color);

        auto canCastleThrough = [&](Square sq1, Square sq2) {
            return board.squares[sq1] == Pieces::None && !isSquareAttackedBy(board, sq1, enemy) &&
                   board.squares[sq2] == Pieces::None && !isSquareAttackedBy(board, sq2, enemy);
        };

        using namespace Squares;
        int offset = board.whiteToMove() ? 0 : 56;
        Square base = E1 + offset;

        if (isSquareAttackedBy(board, base, enemy)) return;

        bool white = board.whiteToMove();
        Board::Castle k_right = white ? Board::white_king_castle : Board::black_king_castle;
        Board::Castle q_right = white ? Board::white_queen_castle : Board::black_queen_castle;

        if (board.canCastle(k_right) && canCastleThrough(F1 + offset, G1 + offset)) {
            moves.push({ base, G1 + offset, MoveType::Castling });
        }

        if (board.canCastle(q_right) && canCastleThrough(D1 + offset, C1 + offset) &&
            board.squares[B1 + offset] == Pieces::None) {
            moves.push({ base, C1 + offset, MoveType::Castling });
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
    Bitboard king_bitboard = board.pieces(Piece::King, color);
    Square king_position = king_bitboard.readBit();
    return isSquareAttackedBy(board, Square(king_position), Piece::flipColor(color));
}

auto generateMoves(const Board& board) -> MoveList {
    MoveList moves;

    // Ordered by mobility - how often you would move that in game
    generateQueenMoves(board, moves, false);
    generateKnightMoves(board, moves, false);
    generatePawnMoves(board, moves);
    generateBishopMoves(board, moves, false);
    generateRookMoves(board, moves, false);
    generateKingMoves(board, moves, false);

    return moves;
}

auto generateCaptures(Board& board) -> MoveList {
    MoveList captures;

    // Ordered by value
    generatePawnCaptures(board, captures);
    generateKnightMoves(board, captures, true);
    generateBishopMoves(board, captures, true);
    generateRookMoves(board, captures, true);
    generateQueenMoves(board, captures, true);
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