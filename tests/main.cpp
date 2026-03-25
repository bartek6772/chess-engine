#include "board.hpp"
#include "diagnostics.hpp"
#include "move_generator.hpp"
#include "precomputed.hpp"
#include <gtest/gtest.h>

#define START_POS "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

TEST(GTestTest, Addition) {
    EXPECT_EQ(10 + 10, 20);
    EXPECT_EQ(3 + 5, 8);
    EXPECT_EQ(-10 + 12, 2);
}

TEST(MoveGeneration, Perft) {
    Board board;
    Precomputed precomputed;
    MoveGenerator move_gen(precomputed);

    board.loadFEN(START_POS);

    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 1), 20);
    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 2), 400);
    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 3), 8902);
    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 4), 197281);
    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 5), 4865609);
    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 6), 119060324);
}