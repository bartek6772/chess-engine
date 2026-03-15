#include "renderer.hpp"
#include "board.hpp"
#include "move_generator.hpp"
#include "pieces.hpp"
#include <raylib.h>
#include <vector>

App::App() : move_gen(precomputed) {
    const char* appDir = GetApplicationDirectory();
    const char* piecesPath = TextFormat("%sassets/pieces.png", appDir);
    pieces_texture = LoadTexture(piecesPath);
}

App::~App() {
    // UnloadTexture(pieces_texture);
}

Rectangle getSource(int piece) {
    int col = 6 - Pieces::pieceType(piece);
    int row = Pieces::isWhite(piece) ? 0 : 1;
    constexpr float size = 2000.0 / 6;
    return { float(col * size), float(row * size), size, size };
}

void App::draw(const Board& board) {
    for (int row = 7; row >= 0; row--) {
        for (int col = 0; col < 8; col++) {
            Color color = (row + col) & 1 ? white_square : black_square;

            Rectangle destination = {
                float(col) * square_size,
                float(7 - row) * square_size,
                square_size,
                square_size,
            };

            DrawRectanglePro(destination, { 0, 0 }, 0, color);

            int square = row * 8 + col;
            if (board.squares[square] != Pieces::None && drag_start != square) {
                Rectangle source = getSource(board.squares[square]);
                DrawTexturePro(pieces_texture, source, destination, { 0, 0 }, 0, WHITE);
            }
        }
    }
}

void App::makeMoveMap(Board& board) {
    available_moves = move_gen.generateLegalMoves(board);

    for (auto& vec : move_map) {
        vec.clear();
    }

    for (const Move& move : available_moves) {
        move_map[move.from].push_back(move.to);
    }
}

void App::drawMoves() {
    if (drag_start == -1) return;

    for (int square : move_map[drag_start]) {
        int row = square / 8;
        int col = square % 8;
        float scale = 0.5F;

        Color circle_color = BLACK;
        circle_color.a = 100;
        DrawCircle(col * square_size + square_size / 2, (7 - row) * square_size + square_size / 2,
            square_size * scale / 2, circle_color);
    }
}

void App::drawDraggedPiece() {
    if (drag_start == -1) return;

    Rectangle dest = {
        (float)GetMouseX() - square_size / 2.0F,
        (float)GetMouseY() - square_size / 2.0F,
        square_size,
        square_size,
    };
    Rectangle source = getSource(dragged_piece);
    DrawTexturePro(pieces_texture, source, dest, { 0, 0 }, 0, WHITE);
}

void App::update(Board& board) {
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && dragged_piece == 0) {
        int file = GetMouseX() / square_size;
        int rank = GetMouseY() / square_size;
        rank = 7 - rank;
        int square = file + rank * 8;

        if (board.squares[square] != Pieces::None) {
            dragged_piece = board.squares[square];
            drag_start = square;
        }
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && dragged_piece != 0) {
        int file = GetMouseX() / square_size;
        int rank = GetMouseY() / square_size;
        rank = 7 - rank;
        int square = file + rank * 8;

        Move final_move;
        for (const Move& move : available_moves) {
            if (move.from == drag_start && move.to == square) {
                final_move = move;
            }
        }

        if (!final_move.isNull()) {
            board.makeMove(final_move);
            makeMoveMap(board);
        }
        drag_start = -1;
        dragged_piece = 0;
    }
}