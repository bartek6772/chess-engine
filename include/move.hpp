#pragma once

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
};