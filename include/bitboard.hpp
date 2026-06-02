#pragma once

#include "square.hpp"
#include <bit>
#include <cstdint>
#include <sys/types.h>

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

    // TODO: Try to come up with and idea how to use constants to represent mask - circular dependency issue
    constexpr Bitboard shift_north() const { return Bitboard(value << 8); }
    constexpr Bitboard shift_south() const { return Bitboard(value >> 8); }
    constexpr Bitboard shift_north_east() const { return Bitboard((value << 9) & 0xfefefefefefefefe); }
    constexpr Bitboard shift_north_west() const { return Bitboard((value << 7) & 0x7f7f7f7f7f7f7f7f); }
    constexpr Bitboard shift_south_east() const { return Bitboard((value >> 7) & 0xfefefefefefefefe); }
    constexpr Bitboard shift_south_west() const { return Bitboard((value >> 9) & 0x7f7f7f7f7f7f7f7f); }

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

    inline bool test(Square square) const {
        return (value & (1ULL << square)) != 0;
    }
};

// clang-format off
constexpr Bitboard operator|(Bitboard lhs, Bitboard rhs) { return Bitboard(lhs.value | rhs.value); }
constexpr Bitboard operator&(Bitboard lhs, Bitboard rhs) { return Bitboard(lhs.value & rhs.value); }
constexpr Bitboard operator^(Bitboard lhs, Bitboard rhs) { return Bitboard(lhs.value ^ rhs.value); }
constexpr bool operator==(Bitboard lhs, Bitboard rhs) { return lhs.value == rhs.value; }
// clang-format on
