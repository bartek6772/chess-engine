#include "board.hpp"
#include "evaluation.hpp"
#include "move_generator.hpp"
#include "square.hpp"
#include <array>

namespace StaticExchange {
namespace {
    Square getLeastValuableAttacker(Bitboard attackers, const Board& board, Piece::Color color) {
        constexpr std::array<Piece::Type, 6> pieces{
            Piece::Pawn,
            Piece::Knight,
            Piece::Bishop,
            Piece::Rook,
            Piece::Queen,
            Piece::King,
        };

        for (auto type : pieces) {
            Bitboard subset = attackers & board.pieces(type, color);
            if (subset) {
                return subset.readBit();
            }
        }

        return Squares::None;
    }

} // namespace

// Move should be a capture but it can not be an enpassant
int SEE(Move move, const Board& board) {
    assert(board.squares[move.to()] != Pieces::None);

    std::array<int, 32> gain{};
    int depth = 0;

    Piece::Color color = board.color_to_move;
    Square square = move.to();
    Square attacker = move.from();

    gain[0] = Evaluation::getPieceValue(board.squares[square]);

    Bitboard occupied = board.pieces();
    Bitboard attackers = MoveGenerator::getAttackers(square, board);

    do {
        attackers ^= Bitboard(attacker);
        occupied ^= Bitboard(attacker);

        depth++;
        gain[depth] = Evaluation::getPieceValue(board.squares[attacker]) - gain[depth - 1];

        color = Piece::flipColor(color);
        Bitboard pieces = board.pieces(color);
        attacker = getLeastValuableAttacker(attackers & pieces, board, color);
        if (attacker == Squares::None) {
            break;
        }

        Bitboard new_attackers = MoveGenerator::getXrayAttackers(square, attacker, occupied, board);
        attackers |= new_attackers & occupied;

    } while (attacker != Squares::None);

    while (--depth > 0) {
        gain[depth - 1] = -std::max(gain[depth], -gain[depth - 1]);
    }

    return gain[0];
}
} // namespace StaticExchange