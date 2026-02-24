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
    void movePiece(int from, int to);

    auto begin() -> int* {
        return list.begin();
    }
    auto end() -> int* {
        return list.begin() + count;
    }

    [[nodiscard]] auto begin() const -> const int* {
        return list.begin();
    }
    [[nodiscard]] auto end() const -> const int* {
        return list.begin() + count;
    }
};