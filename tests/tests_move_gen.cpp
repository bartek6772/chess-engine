#include "board.hpp"
#include "diagnostics.hpp"
#include "move_generator.hpp"
#include "precomputed.hpp"
#include <gtest/gtest.h>

#define START_POS "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

TEST(MoveGeneration, PerftBase) {
    Board board;
    Precomputed precomputed;
    MoveGenerator move_gen(precomputed);

    board.loadFEN(START_POS);

    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 1).total, 20);
    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 2).total, 400);
    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 3).total, 8902);
    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 4).total, 197281);
    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 5).total, 4865609);
    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 6).total, 119060324);
}

TEST(MoveGeneration, PerftPos2) {
    Board board;
    Precomputed precomputed;
    MoveGenerator move_gen(precomputed);

    board.loadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");

    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 1).total, 48);
    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 2).total, 2039);
    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 3).total, 97862);

    Diagnostics::PerftResults perft = Diagnostics::runPerft(board, move_gen, 4);
    EXPECT_EQ(perft.captures, 757163);
    EXPECT_EQ(perft.castles, 128013);
    EXPECT_EQ(perft.enpassants, 1929);
    EXPECT_EQ(perft.total, 4085603);
}

TEST(MoveGeneration, PerftPos3) {
    Board board;
    Precomputed precomputed;
    MoveGenerator move_gen(precomputed);

    board.loadFEN("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");

    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 1).total, 14);
    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 2).total, 191);
    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 3).total, 2812);
    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 4).total, 43238);
    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 5).total, 674624);
    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 6).total, 11030083);
}

TEST(MoveGeneration, PerftPos4) {
    Board board;
    Precomputed precomputed;
    MoveGenerator move_gen(precomputed);

    board.loadFEN("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");

    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 1).total, 6);
    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 2).total, 264);
    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 3).total, 9467);
    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 4).total, 422333);
    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 5).total, 15833292);
    // EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 6).total, 706045033);
}

TEST(MoveGeneration, PerftPos5) {
    Board board;
    Precomputed precomputed;
    MoveGenerator move_gen(precomputed);

    board.loadFEN("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");

    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 1).total, 44);
    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 2).total, 1486);
    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 3).total, 62379);
    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 4).total, 2103487);
    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 5).total, 89941194);
}

TEST(MoveGeneration, PerftPos6) {
    Board board;
    Precomputed precomputed;
    MoveGenerator move_gen(precomputed);

    board.loadFEN("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");

    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 1).total, 46);
    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 2).total, 2079);
    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 3).total, 89890);
    EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 4).total, 3894594);
    // EXPECT_EQ(Diagnostics::runPerft(board, move_gen, 5).total, 164075551);
}