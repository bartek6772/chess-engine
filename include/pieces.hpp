#pragma once

#include <array>
#include <cctype>
#include <string>

namespace Pieces {

inline constexpr int Pawn = 0;
inline constexpr int Knight = 1;
inline constexpr int Bishop = 2;
inline constexpr int Rook = 3;
inline constexpr int Queen = 4;
inline constexpr int King = 5;

inline constexpr int White = 0;
inline constexpr int Black = 1;

inline constexpr auto makePiece(int type, int color) -> int {
    return 2 * type + color;
}

inline constexpr int WhitePawn = makePiece(Pawn, White);
inline constexpr int WhiteKnight = makePiece(Knight, White);
inline constexpr int WhiteBishop = makePiece(Bishop, White);
inline constexpr int WhiteRook = makePiece(Rook, White);
inline constexpr int WhiteQueen = makePiece(Queen, White);
inline constexpr int WhiteKing = makePiece(King, White);

inline constexpr int BlackPawn = makePiece(Pawn, Black);
inline constexpr int BlackKnight = makePiece(Knight, Black);
inline constexpr int BlackBishop = makePiece(Bishop, Black);
inline constexpr int BlackRook = makePiece(Rook, Black);
inline constexpr int BlackQueen = makePiece(Queen, Black);
inline constexpr int BlackKing = makePiece(King, Black);

inline constexpr int None = BlackKing + 1;

inline auto pieceType(int piece) -> int {
    return piece / 2;
}
inline auto pieceColor(int piece) -> int {
    return piece & 1;
}
inline auto isWhite(int piece) -> bool {
    return pieceColor(piece) == White;
}
inline auto isBlack(int piece) -> bool {
    return pieceColor(piece) == Black;
}
inline auto flipColor(int color) {
    return color ^ 1;
}

inline auto getSymbol(int piece) -> char {
    static std::string piece_symbols = "PNBRQK.";
    char sign = piece_symbols[pieceType(piece)];
    return isBlack(piece) ? (char)std::tolower(sign) : sign;
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
    return makePiece(type, color);
}

// clang-format off
static constexpr std::array<int, 12> all_pieces{ 
    WhitePawn, WhiteKnight, WhiteBishop, WhiteRook, WhiteQueen, WhiteKing, 
    BlackPawn, BlackKnight, BlackBishop, BlackRook, BlackQueen, BlackKing 
};

static constexpr std::array<int, 6> white_pieces{ 
    WhitePawn, WhiteKnight, WhiteBishop, WhiteRook, WhiteQueen, WhiteKing, 
};

static constexpr std::array<int, 6> black_pieces{ 
    BlackPawn, BlackKnight, BlackBishop, BlackRook, BlackQueen, BlackKing 
};
// clang-format on

} // namespace Pieces