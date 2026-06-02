#include "board.hpp"
#include "pieces.hpp"
#include "square.hpp"
#include <gtest/gtest.h>

TEST(Fen, Pos1) {
    Board board;
    board.loadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");

    EXPECT_EQ(board.castling_rights, 0b1111);
    EXPECT_EQ(board.whiteToMove(), true);
    EXPECT_EQ(board.enpassant_square, Squares::None);

    // White Pieces
    EXPECT_EQ(board.bitboards[Pieces::WhitePawn.value].value, 0x000000081000E700);
    EXPECT_EQ(board.bitboards[Pieces::WhiteRook.value].value, 0x0000000000000081);
    EXPECT_EQ(board.bitboards[Pieces::WhiteKing.value].value, 0x0000000000000010);
    EXPECT_EQ(board.bitboards[Pieces::WhiteBishop.value].value, 0x0000000000001800);
    EXPECT_EQ(board.bitboards[Pieces::WhiteKnight.value].value, 0x0000001000040000);
    EXPECT_EQ(board.bitboards[Pieces::WhiteQueen.value].value, 0x0000000000200000);

    // Black Pieces
    EXPECT_EQ(board.bitboards[Pieces::BlackPawn.value].value, 0x002D500002800000);
    EXPECT_EQ(board.bitboards[Pieces::BlackRook.value].value, 0x8100000000000000);
    EXPECT_EQ(board.bitboards[Pieces::BlackKing.value].value, 0x1000000000000000);
    EXPECT_EQ(board.bitboards[Pieces::BlackBishop.value].value, 0x0040010000000000);
    EXPECT_EQ(board.bitboards[Pieces::BlackKnight.value].value, 0x0000220000000000);
    EXPECT_EQ(board.bitboards[Pieces::BlackQueen.value].value, 0x0010000000000000);
}

TEST(Fen, Pos2) {
    Board board;
    board.loadFEN("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");

    EXPECT_EQ(board.castling_rights, 0b0000);
    EXPECT_EQ(board.whiteToMove(), true);
    EXPECT_EQ(board.enpassant_square, Squares::None);

    // White Pieces
    EXPECT_EQ(board.bitboards[Pieces::WhitePawn.value].value, 0x0000000200005000);
    EXPECT_EQ(board.bitboards[Pieces::WhiteRook.value].value, 0x0000000002000000);
    EXPECT_EQ(board.bitboards[Pieces::WhiteKing.value].value, 0x0000000100000000);
    EXPECT_EQ(board.bitboards[Pieces::WhiteBishop.value].value, 0x0000000000000000);
    EXPECT_EQ(board.bitboards[Pieces::WhiteKnight.value].value, 0x0000000000000000);
    EXPECT_EQ(board.bitboards[Pieces::WhiteQueen.value].value, 0x0000000000000000);

    // Black Pieces
    EXPECT_EQ(board.bitboards[Pieces::BlackPawn.value].value, 0x0004080020000000);
    EXPECT_EQ(board.bitboards[Pieces::BlackRook.value].value, 0x0000008000000000);
    EXPECT_EQ(board.bitboards[Pieces::BlackKing.value].value, 0x0000000080000000);
    EXPECT_EQ(board.bitboards[Pieces::BlackBishop.value].value, 0x0000000000000000);
    EXPECT_EQ(board.bitboards[Pieces::BlackKnight.value].value, 0x0000000000000000);
    EXPECT_EQ(board.bitboards[Pieces::BlackQueen.value].value, 0x0000000000000000);
}

TEST(Fem, Pos3) {
    Board board;
    board.loadFEN("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");

    EXPECT_EQ(board.castling_rights, 0b1100); // Black kingside & queenside
    EXPECT_EQ(board.whiteToMove(), true);
    EXPECT_EQ(board.enpassant_square, Squares::None);

    // White Pieces
    EXPECT_EQ(board.bitboards[Pieces::WhitePawn.value].value, 0x000100021400C900);
    EXPECT_EQ(board.bitboards[Pieces::WhiteRook.value].value, 0x0000000000000021);
    EXPECT_EQ(board.bitboards[Pieces::WhiteKing.value].value, 0x0000000000000040);
    EXPECT_EQ(board.bitboards[Pieces::WhiteBishop.value].value, 0x0000000003000000);
    EXPECT_EQ(board.bitboards[Pieces::WhiteKnight.value].value, 0x0000800000200000);
    EXPECT_EQ(board.bitboards[Pieces::WhiteQueen.value].value, 0x0000000000000008);

    // Black Pieces
    EXPECT_EQ(board.bitboards[Pieces::BlackPawn.value].value, 0x00EE000000000200);
    EXPECT_EQ(board.bitboards[Pieces::BlackRook.value].value, 0x8100000000000000);
    EXPECT_EQ(board.bitboards[Pieces::BlackKing.value].value, 0x1000000000000000);
    EXPECT_EQ(board.bitboards[Pieces::BlackBishop.value].value, 0x0000420000000000);
    EXPECT_EQ(board.bitboards[Pieces::BlackKnight.value].value, 0x0000200100000000);
    EXPECT_EQ(board.bitboards[Pieces::BlackQueen.value].value, 0x0000000000010000);
}

TEST(Fen, Pos4) {
    Board board;
    board.loadFEN("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");

    EXPECT_EQ(board.castling_rights, 0b0011); // White kingside & queenside
    EXPECT_EQ(board.whiteToMove(), true);
    EXPECT_EQ(board.enpassant_square, Squares::None);

    // White Pieces
    EXPECT_EQ(board.bitboards[Pieces::WhitePawn.value].value, 0x000800000000C700);
    EXPECT_EQ(board.bitboards[Pieces::WhiteRook.value].value, 0x0000000000000081);
    EXPECT_EQ(board.bitboards[Pieces::WhiteKing.value].value, 0x0000000000000010);
    EXPECT_EQ(board.bitboards[Pieces::WhiteBishop.value].value, 0x0000000004000004);
    EXPECT_EQ(board.bitboards[Pieces::WhiteKnight.value].value, 0x0000000000001002);
    EXPECT_EQ(board.bitboards[Pieces::WhiteQueen.value].value, 0x0000000000000008);

    // Black Pieces
    EXPECT_EQ(board.bitboards[Pieces::BlackPawn.value].value, 0x00E3040000000000);
    EXPECT_EQ(board.bitboards[Pieces::BlackRook.value].value, 0x8100000000000000);
    EXPECT_EQ(board.bitboards[Pieces::BlackKing.value].value, 0x2000000000000000);
    EXPECT_EQ(board.bitboards[Pieces::BlackBishop.value].value, 0x0410000000000000);
    EXPECT_EQ(board.bitboards[Pieces::BlackKnight.value].value, 0x0200000000002000);
    EXPECT_EQ(board.bitboards[Pieces::BlackQueen.value].value, 0x0800000000000000);
}

TEST(Fen, Pos5) {
    Board board;
    board.loadFEN("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");

    EXPECT_EQ(board.castling_rights, 0b0000);
    EXPECT_EQ(board.whiteToMove(), true);
    EXPECT_EQ(board.enpassant_square, Squares::None);

    // White Pieces
    EXPECT_EQ(board.bitboards[Pieces::WhitePawn.value].value, 0x000000001009E600);
    EXPECT_EQ(board.bitboards[Pieces::WhiteRook.value].value, 0x0000000000000021);
    EXPECT_EQ(board.bitboards[Pieces::WhiteKing.value].value, 0x0000000000000040);
    EXPECT_EQ(board.bitboards[Pieces::WhiteBishop.value].value, 0x0000004004000000);
    EXPECT_EQ(board.bitboards[Pieces::WhiteKnight.value].value, 0x0000000000240000);
    EXPECT_EQ(board.bitboards[Pieces::WhiteQueen.value].value, 0x0000000000001000);

    // Black Pieces
    EXPECT_EQ(board.bitboards[Pieces::BlackPawn.value].value, 0x00E6091000000000);
    EXPECT_EQ(board.bitboards[Pieces::BlackRook.value].value, 0x2100000000000000);
    EXPECT_EQ(board.bitboards[Pieces::BlackKing.value].value, 0x4000000000000000);
    EXPECT_EQ(board.bitboards[Pieces::BlackBishop.value].value, 0x0000000440000000);
    EXPECT_EQ(board.bitboards[Pieces::BlackKnight.value].value, 0x0000240000000000);
    EXPECT_EQ(board.bitboards[Pieces::BlackQueen.value].value, 0x0010000000000000);
}