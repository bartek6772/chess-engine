#include "board.hpp"
#include "board_renderer.hpp"
#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"
#include "search.hpp"
#include <iostream>
#include <optional>
#include <ostream>
#include <string>

#define START_POS "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

struct App {
    BoardRenderer board_renderer;
    BoardRenderer board_renderer2;
    Board board;
    Board board2;

    int evaluation = 0;

    // Init function
    App() : board_renderer(0, 0, true), board_renderer2(450, 0, false) {
        board.loadFEN(START_POS);
        board_renderer.makeMoveMap(board);

        board2.loadFEN(START_POS);
    }
};

void draw(App& app) {
    app.board_renderer.draw(app.board);
    app.board_renderer2.draw(app.board2);
}

void update(App& app) {
    std::optional<Move> move = app.board_renderer.update(app.board);
    if (move.has_value()) {
        std::cout << "Playing: " << move.value().toString() << std::endl;

        while (app.board.history_ptr < app.board2.history_ptr) {
            app.board2.unmakeMove();
        }

        app.board.makeMove(move.value());
        app.board2.makeMove(move.value());

        Search::SearchResult result = Search::findBestMove(app.board, 4);
        for (Move ai_move : result.pv) {
            app.board2.makeMove(ai_move);
            std::cout << ai_move.toString() << " ";
        }
        std::cout << std::endl;

        app.board.makeMove(result.best_move);
        app.board_renderer.makeMoveMap(app.board);
        app.evaluation = result.score;
    }
}

int main() {

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI | FLAG_VSYNC_HINT);
    InitWindow(1120, 800, "Visualizer");
    SetTargetFPS(60);
    rlImGuiSetup(true);

    App app;

    while (!WindowShouldClose()) {

        update(app);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        draw(app);

        // --- ImGui ---
        rlImGuiBegin();
        ImGui::Begin("Bitboards");
        ImGui::Text("Hello, Engine Developer!");

        ImGui::Text("Evaluation: %d", app.evaluation);

        // if (ImGui::Button("None")) {
        //     app.background_bitboard = BoardRenderer::BackgroundBitbord::None;
        // }
        // if (ImGui::Button("White Attacks")) {
        //     app.background_bitboard = BoardRenderer::BackgroundBitbord::WhiteAttacks;
        // }
        // if (ImGui::Button("Black Attacks")) {
        //     app.background_bitboard = BoardRenderer::BackgroundBitbord::BlackAttacks;
        // }
        // if (ImGui::Button("White Pawns")) {
        //     app.background_bitboard = BoardRenderer::BackgroundBitbord::WhitePawns;
        // }
        // if (ImGui::Button("Black Pawns")) {
        //     app.background_bitboard = BoardRenderer::BackgroundBitbord::BlackPawns;
        // }
        // if (ImGui::Button("Enpassant")) {
        //     app.background_bitboard = BoardRenderer::BackgroundBitbord::Enpassant;
        // }
        ImGui::End();
        rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}