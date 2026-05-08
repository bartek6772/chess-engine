#include "magics.hpp"
#include "constants.hpp"
#include "utility.hpp"
#include <array>
#include <bit>
#include <random>

bitmask calculate_rook_mask(int square) {
    bitmask mask = 0;
    int file = square % 8;
    int rank = square / 8;

    for (int f = file + 1; f <= 6; f++) mask |= setBit(rank * 8 + f);
    for (int f = file - 1; f >= 1; f--) mask |= setBit(rank * 8 + f);

    for (int r = rank + 1; r <= 6; r++) mask |= setBit(r * 8 + file);
    for (int r = rank - 1; r >= 1; r--) mask |= setBit(r * 8 + file);

    return mask;
}

bitmask set_blocker_bits(int index, bitmask mask) {
    bitmask result = 0;
    int bit = 0;
    for (int i = 0; i < 64; i++) {
        if (mask & setBit(i)) {
            if (index & setBit(bit)) {
                result |= setBit(i);
            }
            bit++;
        }
    }
    return result;
}

bitmask calculate_rook_attacks(int square, bitmask blockers) {
    bitmask attacks = 0ULL;
    int file = fileOf(square);
    int rank = rankOf(square);

    // RIGHT
    for (int f = file + 1; f < 8; f++) {
        int sq = rank * 8 + f;
        attacks |= setBit(sq);
        if (blockers & setBit(sq)) break;
    }

    // LEFT
    for (int f = file - 1; f >= 0; f--) {
        int sq = rank * 8 + f;
        attacks |= setBit(sq);
        if (blockers & setBit(sq)) break;
    }

    // UP
    for (int r = rank + 1; r < 8; r++) {
        int sq = r * 8 + file;
        attacks |= setBit(sq);
        if (blockers & setBit(sq)) break;
    }

    // DOWN
    for (int r = rank - 1; r >= 0; r--) {
        int sq = r * 8 + file;
        attacks |= setBit(sq);
        if (blockers & setBit(sq)) break;
    }

    return attacks;
}

bitmask calculate_bishop_mask(int square) {
    bitmask mask = 0ULL;
    int file = square % 8;
    int rank = square / 8;

    int r, f;
    for (r = rank + 1, f = file + 1; r <= 6 && f <= 6; r++, f++) mask |= setBit(r * 8 + f);
    for (r = rank + 1, f = file - 1; r <= 6 && f >= 1; r++, f--) mask |= setBit(r * 8 + f);

    for (r = rank - 1, f = file + 1; r >= 1 && f <= 6; r--, f++) mask |= setBit(r * 8 + f);
    for (r = rank - 1, f = file - 1; r >= 1 && f >= 1; r--, f--) mask |= setBit(r * 8 + f);

    return mask;
}

bitmask calculate_bishop_attack(int square, bitmask blockers) {
    bitmask attack = 0ULL;
    int file = square % 8;
    int rank = square / 8;
    int r, f;

    for (r = rank + 1, f = file + 1; r <= 7 && f <= 7; r++, f++) {
        int sq = r * 8 + f;
        attack |= setBit(sq);
        if (blockers & setBit(sq)) break;
    }
    for (r = rank + 1, f = file - 1; r <= 7 && f >= 0; r++, f--) {
        int sq = r * 8 + f;
        attack |= setBit(sq);
        if (blockers & setBit(sq)) break;
    }

    for (r = rank - 1, f = file + 1; r >= 0 && f <= 7; r--, f++) {
        int sq = r * 8 + f;
        attack |= setBit(sq);
        if (blockers & setBit(sq)) break;
    }
    for (r = rank - 1, f = file - 1; r >= 0 && f >= 0; r--, f--) {
        int sq = r * 8 + f;
        attack |= setBit(sq);
        if (blockers & setBit(sq)) break;
    }

    return attack;
}

bitmask random_u64(std::mt19937_64& rng) {
    bitmask r1 = rng();
    bitmask r2 = rng();
    return (r1 & 0xFFFF'FFFF) | (r2 << 32);
}

bool is_magic_valid(bitmask magic, int relevant_bits, std::vector<bitmask>& blockers,
    std::array<bitmask, 4096>& attacks) {

    int size = 1 << relevant_bits;
    static std::array<bitmask, 4096> used;
    used.fill(0);

    for (int i = 0; i < size; i++) {
        bitmask index = (blockers[i] * magic) >> (BoardSize - relevant_bits);

        if (used[index] != 0 && used[index] != attacks[i]) {
            return false;
        }
        used[index] = attacks[i];
    }

    attacks = used;
    return true;
}

bitmask find_magic(int square, int relevant_bits, std::vector<bitmask>& blockers,
    std::array<bitmask, 4096>& attacks) {
    std::mt19937_64 rng(square + 2026);

    for (int attempts = 0; attempts < 1'000'000; attempts++) {
        bitmask magic = random_u64(rng) & random_u64(rng) & random_u64(rng);

        if (is_magic_valid(magic, relevant_bits, blockers, attacks)) {
            return magic;
        }
    }

    return 0;
}

void Magics::generate() {

    std::vector<bitmask> blockers(4096);

    // Rook
    for (int square = 0; square < BoardSize; square++) {
        // Mask
        bitmask mask = calculate_rook_mask(square);

        // Attacks, blockers
        int bits = std::popcount(mask);
        int total = 1 << bits;

        for (int index = 0; index < total; index++) {
            bitmask blo = set_blocker_bits(index, mask);
            bitmask att = calculate_rook_attacks(square, blo);

            blockers[index] = blo;
            rook_attacks[square][index] = att;
        }

        // Magic
        bitmask magic = find_magic(square, std::popcount(mask), blockers, rook_attacks[square]);

        rook_magics[square] = magic;
        rook_masks[square] = mask;
    }

    blockers.clear();

    // Bishop
    for (int square = 0; square < BoardSize; square++) {
        // Mask
        bitmask mask = calculate_bishop_mask(square);

        // Attacks, blockers
        int bits = std::popcount(mask);
        int total = 1 << bits;

        for (int index = 0; index < total; index++) {
            bitmask blo = set_blocker_bits(index, mask);
            bitmask att = calculate_bishop_attack(square, blo);

            blockers[index] = blo;
            bishop_attacks[square][index] = att;
        }

        // Magic
        bitmask magic = find_magic(square, std::popcount(mask), blockers, bishop_attacks[square]);

        bishop_magics[square] = magic;
        bishop_masks[square] = mask;
    }
}

Magics::Magics() {
    generate();
}