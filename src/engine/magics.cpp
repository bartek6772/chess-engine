#include "magics.hpp"
#include "constants.hpp"
#include "square.hpp"
#include <array>
#include <bit>
#include <cstdint>
#include <random>

inline uint64_t setBit(int bit) {
    return 1ULL << bit;
}

uint64_t calculate_rook_mask(Square square) {
    uint64_t mask = 0;
    int file = square.file();
    int rank = square.rank();

    for (int f = file + 1; f <= 6; f++) mask |= setBit(rank * 8 + f);
    for (int f = file - 1; f >= 1; f--) mask |= setBit(rank * 8 + f);

    for (int r = rank + 1; r <= 6; r++) mask |= setBit(r * 8 + file);
    for (int r = rank - 1; r >= 1; r--) mask |= setBit(r * 8 + file);

    return mask;
}

uint64_t set_blocker_bits(int index, uint64_t mask) {
    uint64_t result = 0;
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

uint64_t calculate_rook_attacks(Square square, uint64_t blockers) {
    uint64_t attacks = 0ULL;
    int file = square.file();
    int rank = square.rank();

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

uint64_t calculate_bishop_mask(Square square) {
    uint64_t mask = 0ULL;
    int file = square.file();
    int rank = square.rank();

    int r, f;
    for (r = rank + 1, f = file + 1; r <= 6 && f <= 6; r++, f++) mask |= setBit(r * 8 + f);
    for (r = rank + 1, f = file - 1; r <= 6 && f >= 1; r++, f--) mask |= setBit(r * 8 + f);

    for (r = rank - 1, f = file + 1; r >= 1 && f <= 6; r--, f++) mask |= setBit(r * 8 + f);
    for (r = rank - 1, f = file - 1; r >= 1 && f >= 1; r--, f--) mask |= setBit(r * 8 + f);

    return mask;
}

uint64_t calculate_bishop_attack(Square square, uint64_t blockers) {
    uint64_t attack = 0ULL;
    int file = square.file();
    int rank = square.rank();
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

uint64_t random_u64(std::mt19937_64& rng) {
    uint64_t r1 = rng();
    uint64_t r2 = rng();
    return (r1 & 0xFFFF'FFFF) | (r2 << 32);
}

bool is_magic_valid(uint64_t magic, int relevant_bits, std::vector<uint64_t>& blockers,
    std::array<uint64_t, 4096>& attacks) {

    int size = 1 << relevant_bits;
    static std::array<uint64_t, 4096> used;
    used.fill(0);

    for (int i = 0; i < size; i++) {
        uint64_t index = (blockers[i] * magic) >> (BoardSize - relevant_bits);

        if (used[index] != 0 && used[index] != attacks[i]) {
            return false;
        }
        used[index] = attacks[i];
    }

    attacks = used;
    return true;
}

uint64_t find_magic(Square square, int relevant_bits, std::vector<uint64_t>& blockers,
    std::array<uint64_t, 4096>& attacks) {
    std::mt19937_64 rng(square + 2026);

    for (int attempts = 0; attempts < 1'000'000; attempts++) {
        uint64_t magic = random_u64(rng) & random_u64(rng) & random_u64(rng);

        if (is_magic_valid(magic, relevant_bits, blockers, attacks)) {
            return magic;
        }
    }

    return 0;
}

void Magics::generate() {

    std::vector<uint64_t> blockers(4096);

    // Rook
    for (int sq = 0; sq < BoardSize; sq++) {
        Square square(sq);

        // Mask
        uint64_t mask = calculate_rook_mask(square);

        // Attacks, blockers
        int bits = std::popcount(mask);
        int total = 1 << bits;

        for (int index = 0; index < total; index++) {
            uint64_t blo = set_blocker_bits(index, mask);
            uint64_t att = calculate_rook_attacks(square, blo);

            blockers[index] = blo;
            rook_attacks[sq][index] = att;
        }

        // Magic
        uint64_t magic = find_magic(square, bits, blockers, rook_attacks[sq]);

        rook_magics[sq] = magic;
        rook_masks[sq] = mask;
    }

    blockers.clear();

    // Bishop
    for (int sq = 0; sq < BoardSize; sq++) {
        Square square(sq);
        // Mask
        uint64_t mask = calculate_bishop_mask(square);

        // Attacks, blockers
        int bits = std::popcount(mask);
        int total = 1 << bits;

        for (int index = 0; index < total; index++) {
            uint64_t blo = set_blocker_bits(index, mask);
            uint64_t att = calculate_bishop_attack(square, blo);

            blockers[index] = blo;
            bishop_attacks[square][index] = att;
        }

        // Magic
        uint64_t magic = find_magic(square, bits, blockers, bishop_attacks[sq]);

        bishop_magics[sq] = magic;
        bishop_masks[sq] = mask;
    }
}

Magics::Magics() {
    generate();
}