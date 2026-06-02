#pragma once

#include <cassert>

struct Square {
    int value;

    constexpr Square() : value(){};
    constexpr Square(int file, int rank) : value(rank * 8 + file) {}
    explicit constexpr Square(int val) : value(val) {}

    inline int file() const {
        assert(value >= 0 && value <= 63);
        return value & 7;
    }

    inline int rank() const {
        assert(value >= 0 && value <= 63);
        return value >> 3;
    }

    constexpr operator int() const {
        return value;
    }
};

inline Square operator+(Square square, int offset) {
    return Square(square.value + offset);
}

inline Square operator-(Square square, int offset) {
    return Square(square.value - offset);
}

// NOLINTBEGIN
namespace Squares {

constexpr Square A1 = { 0, 0 };
constexpr Square B1 = { 1, 0 };
constexpr Square C1 = { 2, 0 };
constexpr Square D1 = { 3, 0 };
constexpr Square E1 = { 4, 0 };
constexpr Square F1 = { 5, 0 };
constexpr Square G1 = { 6, 0 };
constexpr Square H1 = { 7, 0 };

constexpr Square A8 = { 0, 7 };
constexpr Square B8 = { 1, 7 };
constexpr Square C8 = { 2, 7 };
constexpr Square D8 = { 3, 7 };
constexpr Square E8 = { 4, 7 };
constexpr Square F8 = { 5, 7 };
constexpr Square G8 = { 6, 7 };
constexpr Square H8 = { 7, 7 };

constexpr Square None = Square(64);

} // namespace Squares
// NOLINTEND