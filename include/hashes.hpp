#pragma once

#include "constants.hpp"
#include <array>
#include <random>

struct Hashes {
    std::array<std::array<uint64_t, BoardSize>, MaxPiecesCount> piece_square{};
    std::array<uint64_t, BoardLength> enpassant_file{};
    std::array<uint64_t, 16> castling_rights{};
    uint64_t side_to_move;

    Hashes() {
        std::mt19937_64 rng(2026);

        for (auto& piece : piece_square) {
            for (auto& square : piece) {
                square = rng();
            }
        }

        for (auto& castling : castling_rights) {
            castling = rng();
        }

        for (auto& ep : enpassant_file) {
            ep = rng();
        }

        side_to_move = rng();
    }
};