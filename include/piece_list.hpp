#pragma once

#include "constants.hpp"
#include <array>

constexpr int MaxPiecesCount = 16;

class PieceList {
private:
    std::array<int, MaxPiecesCount> list{};
    std::array<int, BoardSize> map{};
    int count = 0;

public:
    void addPiece(int square);
    void removePiece(int square);

    auto begin() -> int* { return list.begin(); }
    auto end() -> int* { return list.begin() + count; }

    auto begin() const -> const int* { return list.begin(); }
    auto end() const -> const int* { return list.begin() + count; }
};