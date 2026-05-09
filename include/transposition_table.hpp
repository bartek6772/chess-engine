#pragma once

#include "move.hpp"
#include <cmath>
#include <cstdint>
#include <vector>

enum NodeFlag : uint8_t { EXACT, LOWER_BOUND, UPPER_BOUND };

struct TTEntry {
    uint64_t hash;
    int16_t depth;
    int16_t score;
    NodeFlag flag;
    Move best_move;
};

class TranspositionTable {
private:
    std::vector<TTEntry> table;
    int size;
    int used_entries;

    inline int indexFrom(unsigned long long hash) const {
        return static_cast<int>(hash & (size - 1));
    }

public:
    TranspositionTable(int size_mb) {
        int entries_limit = (size_mb * 1024 * 1024) / sizeof(TTEntry);

        int entries_pow_2 = 1;
        while (entries_pow_2 < entries_limit / 2) {
            entries_pow_2 <<= 1;
        }

        table.resize(entries_pow_2);
        size = entries_pow_2;

        used_entries = 0;
    }

    const TTEntry* get(unsigned long long hash) const {
        int index = indexFrom(hash);
        if (table[index].hash == hash) {
            return &table[index];
        }
        return nullptr;
    }

    void store(const TTEntry& entry) {
        int index = indexFrom(entry.hash);
        if (table[index].hash == 0) {
            used_entries++;
        }

        table[index] = entry;
    };

    void clear() {
        table.clear();
        used_entries = 0;
    }

    int fillRate() const {
        return used_entries * 100 / size;
    }
};