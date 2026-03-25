#pragma once

#include "constants.hpp"
#include <array>

class Magics {
public:
    inline bitmask getRookAttacks(int square, bitmask blockers) const {
        return rook_attacks[square][(blockers * rook_magics[square]) >> (64 - RBits[square])];
    }

    inline bitmask getBishopAttacks(int square, bitmask blockers) const {
        return bishop_attacks[square][(blockers * bishop_magics[square]) >> (64 - BBits[square])];
    }

    inline bitmask getRookMask(int square) const {
        return rook_masks[square];
    }

    inline bitmask getBishopMask(int square) const {
        return bishop_masks[square];
    }

    Magics();

private:
    void load();
    void generate();

    std::array<bitmask, BoardSize> rook_magics{};
    std::array<bitmask, BoardSize> rook_masks{};
    std::array<std::array<bitmask, 4096>, BoardSize> rook_attacks{};

    std::array<bitmask, BoardSize> bishop_magics{};
    std::array<bitmask, BoardSize> bishop_masks{};
    std::array<std::array<bitmask, 4096>, BoardSize> bishop_attacks{};

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