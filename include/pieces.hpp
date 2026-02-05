#pragma once

#include <cctype>
namespace Pieces {

inline constexpr int None = 0;
inline constexpr int Pawn = 1;
inline constexpr int Rook = 2;
inline constexpr int Knight = 3;
inline constexpr int Bishop = 4;
inline constexpr int Queen = 5;
inline constexpr int King = 6;

inline constexpr int White = 0;
inline constexpr int Black = 8;

inline constexpr int WhitePawn = Pawn | White;
inline constexpr int WhiteKnight = Knight | White;
inline constexpr int WhiteBishop = Bishop | White;
inline constexpr int WhiteRook = Rook | White;
inline constexpr int WhiteQueen = Queen | White;
inline constexpr int WhiteKing = King | White;

inline constexpr int BlackPawn = Pawn | Black;
inline constexpr int BlackKnight = Knight | Black;
inline constexpr int BlackBishop = Bishop | Black;
inline constexpr int BlackRook = Rook | Black;
inline constexpr int BlackQueen = Queen | Black;
inline constexpr int BlackKing = King | Black;

inline constexpr int MaxPieceIndex = BlackKing;

inline constexpr int TypeMask = 0b0111;
inline constexpr int ColorMask = 0b1000;

inline auto pieceType(const int piece) -> int { return piece & TypeMask; }
inline auto pieceColor(const int piece) -> int { return piece & ColorMask; }

inline auto getSymbol(int piece) -> char {
    const char* piece_symbols = ".PRNBQK";
    char sign = piece_symbols[pieceType(piece)];
    return pieceColor(piece) == Black ? (char)std::tolower(sign) : sign;
}

} // namespace Pieces