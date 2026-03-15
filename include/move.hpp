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
        int row = from / BoardLength;
        int col = from % BoardLength;
        move += (char)(col + 'a');
        move += (char)(row + '1');

        row = to / BoardLength;
        col = to % BoardLength;
        move += (char)(col + 'a');
        move += (char)(row + '1');

        return move;
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
};