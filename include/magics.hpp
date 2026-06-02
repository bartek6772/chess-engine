#pragma once

#include "bitboard.hpp"
#include "constants.hpp"
#include "square.hpp"
#include <array>
#include <cstdint>

class Magics {
public:
    inline Bitboard getRookAttacks(Square square, uint64_t blockers) const {
        uint64_t tmp =
            rook_attacks[square][(blockers * rook_magics[square]) >> (64 - RBits[square])];
        return Bitboard(tmp);
    }

    inline Bitboard getBishopAttacks(Square square, uint64_t blockers) const {
        uint64_t tmp =
            bishop_attacks[square][(blockers * bishop_magics[square]) >> (64 - BBits[square])];
        return Bitboard(tmp);
    }

    inline Bitboard getRookMask(Square square) const {
        return Bitboard(rook_masks[square]);
    }

    inline Bitboard getBishopMask(Square square) const {
        return Bitboard(bishop_masks[square]);
    }

    Magics();

private:
    void load();
    void generate();

    std::array<uint64_t, BoardSize> rook_magics{};
    std::array<uint64_t, BoardSize> rook_masks{};
    std::array<std::array<uint64_t, 4096>, BoardSize> rook_attacks{};

    std::array<uint64_t, BoardSize> bishop_magics{};
    std::array<uint64_t, BoardSize> bishop_masks{};
    std::array<std::array<uint64_t, 4096>, BoardSize> bishop_attacks{};

    // clang-format off
    inline static constexpr int RBits[64] = {
		12, 11, 11, 11, 11, 11, 11, 12,
		11, 10, 10, 10, 10, 10, 10, 11,
		11, 10, 10, 10, 10, 10, 10, 11,
		11, 10, 10, 10, 10, 10, 10, 11,
		11, 10, 10, 10, 10, 10, 10, 11,
		11, 10, 10, 10, 10, 10, 10, 11,
		11, 10, 10, 10, 10, 10, 10, 11,
		12, 11, 11, 11, 11, 11, 11, 12
	};

	inline static constexpr int BBits[64] = {
		6, 5, 5, 5, 5, 5, 5, 6,
		5, 5, 5, 5, 5, 5, 5, 5,
		5, 5, 7, 7, 7, 7, 5, 5,
		5, 5, 7, 9, 9, 7, 5, 5,
		5, 5, 7, 9, 9, 7, 5, 5,
		5, 5, 7, 7, 7, 7, 5, 5,
		5, 5, 5, 5, 5, 5, 5, 5,
		6, 5, 5, 5, 5, 5, 5, 6
	};
    // clang-format on
};