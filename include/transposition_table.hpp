#pragma once

#include "move.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>

enum NodeFlag : uint8_t { EXACT, LOWER_BOUND, UPPER_BOUND };

struct TTEntry {
    uint64_t hash = 0; // 8
    int16_t score = 0; // 2
    uint8_t depth = 0; // 1
    NodeFlag flag{};   // 1
    Move best_move;    // 4
};

class TranspositionTable {
private:
    std::vector<TTEntry> table;
    int size;
    int used_entries;

    inline size_t indexFrom(unsigned long long hash) const {
        return (static_cast<unsigned __int128>(hash) * size) >> 64;
    }

public:
    TranspositionTable(int size_mb) {
        constexpr int MB_TO_BYTES = 1024 * 1024;
        size = static_cast<int>((1LL * size_mb * MB_TO_BYTES) / sizeof(TTEntry));

        table.resize(size);
        used_entries = 0;
    }

    const TTEntry* get(unsigned long long hash) const {
        size_t index = indexFrom(hash);
        if (table[index].hash == hash) {
            return &table[index];
        }
        return nullptr;
    }

    void store(const TTEntry& entry) {
        size_t index = indexFrom(entry.hash);
        if (table[index].hash == 0) {
            used_entries++;
        }

        table[index] = entry;
    };

    void clear() {
        table.assign(size, {});
        used_entries = 0;
    }

    int fillRate() const {
        return used_entries * 100 / size;
    }
};