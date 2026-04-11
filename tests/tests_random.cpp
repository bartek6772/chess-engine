#include "board.hpp"
#include "move_generator.hpp"
#include "pieces.hpp"
#include <gtest/gtest.h>

TEST(Misc, CheckmateDetection_Rh8) {
    // Position: Black King on a8, White Rook on h8 (Checkmate)
    Board board;
    board.loadFEN("k6R/p2Q1K2/p4N2/2B5/1P6/8/8/1r6 b - - 0 1");

    EXPECT_EQ(MoveGenerator::isCheck(board, Pieces::White), false);
    EXPECT_EQ(MoveGenerator::isCheck(board, Pieces::Black), true);

    auto legalMoves = MoveGenerator::generateLegalMoves(board);
    EXPECT_EQ(legalMoves.size(), 0)
        << "Bug: Engine found " << legalMoves.size() << " legal moves in a checkmate position.";

    if (legalMoves.size() > 0) {
        for (const auto& move : legalMoves) {
            std::cout << "Illegal move found: " << move.toString() << std::endl;
        }
    }
}

TEST(Misc, ReloadingFEN) {
    Board board;
    board.loadStartPos();

    Move e2e4;
    auto moves = MoveGenerator::generateLegalMoves(board);
    for (const auto& m : moves) {
        if (m.from == 12 && m.to == 28) {
            e2e4 = m;
            break;
        }
    }
    ASSERT_FALSE(e2e4.isNull());

    board.makeMove(e2e4);
    board.loadStartPos();

    uint64_t e4_bit = 1ULL << 28;
    EXPECT_EQ(board.white_pieces & e4_bit, 0ULL)
        << "White pieces should not have e4 set after reloading startpos";
}