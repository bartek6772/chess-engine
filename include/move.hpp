#pragma once

#include "square.hpp"
#include <cassert>
#include <string>

// +-----------------+------+--------+--------+
// |      Score      | Type |   To   |  From  |
// |       16b       |  4b  |   6b   |   6b   |
// +-----------------+------+--------+--------+

enum class MoveType : int {
    Normal,
    DoublePush,
    Castling,
    EnPassant,
    PromotionKnight,
    PromotionBishop,
    PromotionRook,
    PromotionQueen,
};

struct Move {
    unsigned int data;

    static constexpr int square_mask = 0x3F;
    static constexpr int type_mask = 0xF;
    static constexpr int half_mask = 0xFFFF;

    inline Square from() const {
        return Square(data & square_mask);
    }

    inline Square to() const {
        return Square((data >> 6) & square_mask);
    }

    inline MoveType type() const {
        return static_cast<MoveType>((data >> 12) & type_mask);
    }

    inline int score() const {
        return (data >> 16) & half_mask;
    }

    [[nodiscard]] auto toString() const -> std::string {
        std::string move{};
        move += (char)(from().file() + 'a');
        move += (char)(from().rank() + '1');

        move += (char)(to().file() + 'a');
        move += (char)(to().rank() + '1');

        if (isPromotion()) {
            switch (type()) {
                case MoveType::PromotionQueen: move += 'q'; break;
                case MoveType::PromotionRook: move += 'r'; break;
                case MoveType::PromotionBishop: move += 'b'; break;
                case MoveType::PromotionKnight: move += 'n'; break;
                default: break;
            }
        }
        return move;
    }

    Move() : data(0) {}

    Move(Square from, Square to, MoveType type = MoveType::Normal) {
        data = (from) | (to << 6) | ((int)type << 12);
    }

    void setScore(int score) {
        assert(score < (1 << 17 - 1));
        data = (score << 16) | (data & half_mask);
    }

    auto operator==(const Move other) const -> bool {
        return (data & half_mask) == (other.data & half_mask);
    }

    auto operator!=(const Move other) const -> bool {
        return (data & half_mask) != (other.data & half_mask);
    }

    auto operator>(const Move other) const -> bool {
        return data > other.data;
    }

    bool isNull() const {
        return data == 0;
    }

    bool isPromotion() const {
        switch (type()) {
            case MoveType::PromotionQueen:
            case MoveType::PromotionRook:
            case MoveType::PromotionBishop:
            case MoveType::PromotionKnight: return true;
            default: return false;
        }
    }
};