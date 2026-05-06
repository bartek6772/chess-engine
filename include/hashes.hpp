#pragma once

#include "constants.hpp"
#include "pieces.hpp"
#include <array>
#include <random>

struct Hashes {
    std::array<std::array<unsigned long long, BoardSize>, Pieces::MaxPieceIndex> piece_square{};
    std::array<unsigned long long, BoardLength> enpassant_file{};
    std::array<unsigned long long, 16> castling_rights{};
    unsigned long long side_to_move;

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