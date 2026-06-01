#pragma once

#include <cctype>
#include <cstdint>

struct Piece {

    enum Type : uint8_t {
        Pawn = 0,
        Knight = 1,
        Bishop = 2,
        Rook = 3,
        Queen = 4,
        King = 5,
        None = 6
    };

    enum Color : uint8_t {
        White = 0,
        Black = 1,
    };

    uint8_t value;
    constexpr Piece() : value(2 * None + White) {}
    constexpr Piece(Type type, Color color) : value(2 * type + color) {}

    inline Type type() const {
        return static_cast<Type>(value >> 1);
    }

    inline Color color() const {
        return static_cast<Color>(value & 1);
    }

    inline bool isWhite() const {
        return color() == Color::White;
    }

    inline bool isBlack() const {
        return color() == Color::Black;
    }

    inline char getSymbol() const {
        static constexpr char piece_symbols[] = "PNBRQK.";
        char sign = piece_symbols[type()];
        return isBlack() ? static_cast<char>(std::tolower(sign)) : sign;
    }

    inline static Color flipColor(Color color) {
        return static_cast<Color>(color ^ 1);
    }

    inline static Piece getPiece(char symbol) {
        Type type = None;

        switch ((char)std::toupper(symbol)) {
            case 'P': type = Pawn; break;
            case 'R': type = Rook; break;
            case 'N': type = Knight; break;
            case 'B': type = Bishop; break;
            case 'Q': type = Queen; break;
            case 'K': type = King; break;
        }

        Color color = std::islower(symbol) ? Black : White;
        return { type, color };
    }

    inline bool operator==(const Piece other) const {
        return value == other.value;
    }

    inline bool operator!=(const Piece other) const {
        return value != other.value;
    }
};

namespace Pieces {

inline constexpr Piece WhitePawn = { Piece::Pawn, Piece::White };
inline constexpr Piece WhiteKnight = { Piece::Knight, Piece::White };
inline constexpr Piece WhiteBishop = { Piece::Bishop, Piece::White };
inline constexpr Piece WhiteRook = { Piece::Rook, Piece::White };
inline constexpr Piece WhiteQueen = { Piece::Queen, Piece::White };
inline constexpr Piece WhiteKing = { Piece::King, Piece::White };

inline constexpr Piece BlackPawn = { Piece::Pawn, Piece::Black };
inline constexpr Piece BlackKnight = { Piece::Knight, Piece::Black };
inline constexpr Piece BlackBishop = { Piece::Bishop, Piece::Black };
inline constexpr Piece BlackRook = { Piece::Rook, Piece::Black };
inline constexpr Piece BlackQueen = { Piece::Queen, Piece::Black };
inline constexpr Piece BlackKing = { Piece::King, Piece::Black };

inline constexpr Piece None = { Piece::None, Piece::White };

} // namespace Pieces