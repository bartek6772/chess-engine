#pragma once

#include "move.hpp"
#include <vector>

constexpr int TT_BITS = 20;
constexpr int TT_SIZE = 1 << TT_BITS;
constexpr int TT_MASK = TT_SIZE - 1;

enum NodeFlag { EXACT, LOWER_BOUND, UPPER_BOUND };

struct TTEntry {
    unsigned long long hash;
    int depth;
    int score;
    NodeFlag flag;
    Move best_move;
};

class TranspositionTable {
private:
    std::vector<TTEntry> table;

    static inline int indexFrom(unsigned long long hash) {
        return static_cast<int>(hash & TT_MASK);
    }

public:
    TranspositionTable() : table(TT_SIZE) {
    }

    const TTEntry* get(unsigned long long hash) const {
        int index = indexFrom(hash);
        if (table[index].hash == hash) {
            return &table[index];
        }
        return nullptr;
    }

    void store(const TTEntry& entry) {
        table[indexFrom(entry.hash)] = entry;
    };
};