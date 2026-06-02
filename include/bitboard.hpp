#pragma once

#include "square.hpp"
#include <bit>
#include <cstdint>

struct Bitboard {
    uint64_t value;

    constexpr Bitboard() : value() {}
    explicit constexpr Bitboard(uint64_t val) : value(val) {}
    explicit constexpr Bitboard(Square val) : value(1ULL << val) {}

    // clang-format off
    constexpr Bitboard& operator|=(Bitboard rhs) { value |= rhs.value; return *this; }
    constexpr Bitboard& operator&=(Bitboard rhs) { value &= rhs.value; return *this; }
    constexpr Bitboard& operator^=(Bitboard rhs) { value ^= rhs.value; return *this; }

    constexpr Bitboard operator~() const { return Bitboard(~value); }
    explicit operator bool() const { return value != 0; }
    // clang-format on

    inline void clear() {
        value = 0ULL;
    }

    inline Square readBit() {
        assert(value != 0);
        int bit = std::countr_zero(value);
        value &= (value - 1);
        return Square(bit);
    }

    inline int count() const {
        return std::popcount(value);
    }
};

// clang-format off
constexpr Bitboard operator|(Bitboard lhs, Bitboard rhs) { return Bitboard(lhs.value | rhs.value); }
constexpr Bitboard operator&(Bitboard lhs, Bitboard rhs) { return Bitboard(lhs.value & rhs.value); }
constexpr Bitboard operator^(Bitboard lhs, Bitboard rhs) { return Bitboard(lhs.value ^ rhs.value); }
constexpr bool operator==(Bitboard lhs, Bitboard rhs) { return lhs.value == rhs.value; }
// clang-format on
