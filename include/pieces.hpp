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
inline auto isWhite(const int piece) -> bool { return pieceColor(piece) == White; }
inline auto isBlack(const int piece) -> bool { return pieceColor(piece) == Black; }

inline auto getSymbol(int piece) -> char {
    const char* piece_symbols = ".PRNBQK";
    char sign = piece_symbols[pieceType(piece)];
    return pieceColor(piece) == Black ? (char)std::tolower(sign) : sign;
}

inline auto getPiece(char symbol) -> int {
    int type = Pieces::None;

    switch ((char)std::toupper(symbol)) {
        case 'P': type = Pieces::Pawn; break;
        case 'R': type = Pieces::Rook; break;
        case 'N': type = Pieces::Knight; break;
        case 'B': type = Pieces::Bishop; break;
        case 'Q': type = Pieces::Queen; break;
        case 'K': type = Pieces::King; break;
    }

    int color = std::islower(symbol) ? Pieces::Black : Pieces::White;
    return type | color;
}

} // namespace Pieces