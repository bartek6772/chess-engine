#pragma once
#include "board.hpp"
#include "move.hpp"
#include "move_generator.hpp"
#include "precomputed.hpp"
#include <array>
#include <raylib.h>
#include <vector>

constexpr Color white_square = Color(238, 238, 210, 255);
constexpr Color black_square = Color(118, 150, 86, 255);
constexpr int square_size = 70;

class App {
public:
    App();
    ~App();

    void update(Board& board);

    void draw(const Board& board);
    void drawMoves();
    void drawDraggedPiece();

    void makeMoveMap(Board& board);

private:
    Texture2D pieces_texture;
    std::array<std::vector<int>, 64> move_map;
    std::vector<Move> available_moves;

    Precomputed precomputed;
    MoveGenerator move_gen;

    int drag_start = -1;
    int dragged_piece = 0;
};