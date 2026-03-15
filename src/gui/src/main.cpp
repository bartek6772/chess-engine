#include "board.hpp"
#include "imgui.h"
#include "move.hpp"
#include "move_generator.hpp"
#include "raylib.h"
#include "rlImGui.h"
#include "src/renderer.hpp"
#include <array>
#include <vector>

int main() {

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI | FLAG_VSYNC_HINT);
    InitWindow(1120, 800, "Visualizer");
    SetTargetFPS(60);
    rlImGuiSetup(true);

    Board board;

    board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    App renderer;
    renderer.makeMoveMap(board);

    while (!WindowShouldClose()) {

        BeginDrawing();

        // --- Raylib ---
        ClearBackground(RAYWHITE);
        renderer.update(board);
        renderer.draw(board);
        renderer.drawMoves();
        renderer.drawDraggedPiece();

        // --- ImGui ---
        rlImGuiBegin();
        ImGui::Begin("Engine debugger");
        ImGui::Text("Hello, Engine Developer!");
        if (ImGui::Button("Reset Board")) {
        }
        ImGui::End();
        rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}