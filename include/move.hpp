#pragma once

#include "constants.hpp"
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
    int from;
    int to;
    MoveType type;

    [[nodiscard]] auto toString() const -> std::string {
        std::string move{};
        move += (char)(from % BoardLength + 'a');
        move += (char)(from / BoardLength + '1');

        move += (char)(to % BoardLength + 'a');
        move += (char)(to / BoardLength + '1');

        if (isPromotion()) {
            switch (type) {
                case MoveType::PromotionRook: move += 'r'; break;
                case MoveType::PromotionBishop: move += 'b'; break;
                case MoveType::PromotionKnight: move += 'n'; break;
                case MoveType::PromotionQueen: move += 'q'; break;
                default: break;
            }
        }
        return move;
    }

    Move() : from(0), to(0), type(MoveType::Normal) {
    }

    Move(int from, int to, MoveType type = MoveType::Normal) : from(from), to(to), type(type) {
    }

    Move(const std::string& move) {
        from = (move[0] - 'a') + (move[1] - '1') * BoardLength;
        to = (move[2] - 'a') + (move[3] - '1') * BoardLength;
        type = MoveType::Normal;
    }

    auto operator==(Move& move) const -> bool {
        return from == move.from && to == move.to && type == move.type;
    }

    auto operator!=(Move& move) const -> bool {
        return from != move.from || to != move.to || type != move.type;
    }

    bool isNull() const {
        return to == 0 && from == 0;
    }

    bool isPromotion() const {
        return type == MoveType::PromotionBishop || type == MoveType::PromotionKnight ||
               type == MoveType::PromotionQueen || type == MoveType::PromotionRook;
    }
};