#include "app.hpp"
#include "board.hpp"
#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"

int main() {

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI | FLAG_VSYNC_HINT);
    InitWindow(1120, 800, "Visualizer");
    SetTargetFPS(60);
    rlImGuiSetup(true);

    Board board;

    board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    App app;
    app.makeMoveMap(board);

    while (!WindowShouldClose()) {

        BeginDrawing();

        // --- Raylib ---
        ClearBackground(RAYWHITE);
        app.update(board);
        app.draw(board);
        app.drawMoves();
        app.drawDraggedPiece();

        // --- ImGui ---
        rlImGuiBegin();
        ImGui::Begin("Bitboards");
        ImGui::Text("Hello, Engine Developer!");
        if (ImGui::Button("None")) {
            app.background_bitboard = App::BackgroundBitbord::None;
        }
        if (ImGui::Button("White Attacks")) {
            app.background_bitboard = App::BackgroundBitbord::WhiteAttacks;
        }
        if (ImGui::Button("Black Attacks")) {
            app.background_bitboard = App::BackgroundBitbord::BlackAttacks;
        }
        if (ImGui::Button("White Pawns")) {
            app.background_bitboard = App::BackgroundBitbord::WhitePawns;
        }
        if (ImGui::Button("Black Pawns")) {
            app.background_bitboard = App::BackgroundBitbord::BlackPawns;
        }
        ImGui::End();
        rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}