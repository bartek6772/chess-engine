#pragma once
#include "board.hpp"
#include "move.hpp"
#include "move_list.hpp"
#include <array>
#include <optional>
#include <raylib.h>
#include <vector>

constexpr Color white_square = Color(238, 238, 210, 255);
constexpr Color black_square = Color(118, 150, 86, 255);
constexpr Color bitbord_overlay = Color(68, 111, 212, 200);
constexpr Color last_move_overlay = Color(224, 190, 20, 150);

constexpr int square_size = 50;

class BoardRenderer {
public:
    BoardRenderer(int pos_x, int pos_y, bool interactable);
    ~BoardRenderer();

    std::optional<Move> update(Board& board);
    void draw(const Board& board);

    void makeMoveMap(Board& board);

    enum class BackgroundBitbord {
        None,
        // WhiteAttacks,
        // BlackAttacks,
        WhitePawns,
        BlackPawns,
        Enpassant,
    };
    BackgroundBitbord background_bitboard;

private:
    Texture2D pieces_texture;
    std::array<std::vector<int>, 64> move_map;
    MoveList available_moves;

    int pos_x;
    int pos_y;

    void drawMoves();
    void drawDraggedPiece();
    void drawBoard(const Board& board);

    int drag_start = -1;
    int dragged_piece = 0;

    bool interactable;
};