#pragma once

#include "square.hpp"
#include <cstdint>
#include <string>

enum class MoveType : int {
    Normal,
    DoublePush,
    Castling,
    EnPassant,
    PromotionRook,
    PromotionBishop,
    PromotionKnight,
    PromotionQueen,
};

struct Move {
    uint16_t data;
    uint16_t score;

    static const int from_mask = 0b0000000000111111;
    static const int to_mask = 0b0000111111000000;
    static const int flag_mask = 0b1111000000000000;

    inline Square from() const {
        return Square(data & from_mask);
    }

    inline Square to() const {
        return Square((data & to_mask) >> 6);
    }

    inline MoveType type() const {
        return static_cast<MoveType>((data & flag_mask) >> 12);
    }

    [[nodiscard]] auto toString() const -> std::string {
        std::string move{};
        move += (char)(from().file() + 'a');
        move += (char)(from().rank() + '1');

        move += (char)(to().file() + 'a');
        move += (char)(to().rank() + '1');

        if (isPromotion()) {
            switch (type()) {
                case MoveType::PromotionRook: move += 'r'; break;
                case MoveType::PromotionBishop: move += 'b'; break;
                case MoveType::PromotionKnight: move += 'n'; break;
                case MoveType::PromotionQueen: move += 'q'; break;
                default: break;
            }
        }
        return move;
    }

    Move() : data(0){};

    Move(Square from, Square to, MoveType type = MoveType::Normal) {
        data = (from) | (to << 6) | ((int)type << 12);
    }

    auto operator==(const Move& other) const -> bool {
        return data == other.data;
    }

    auto operator!=(const Move& other) const -> bool {
        return data != other.data;
    }

    bool isNull() const {
        return data == 0;
    }

    bool isPromotion() const {
        return type() == MoveType::PromotionBishop || type() == MoveType::PromotionKnight ||
               type() == MoveType::PromotionQueen || type() == MoveType::PromotionRook;
    }
};