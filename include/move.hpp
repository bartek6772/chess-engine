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

    auto toString() -> std::string {
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

    // auto operator<<(std::ostream& stream) -> std::ostream& {
    //     return stream << toString();
    // }
};