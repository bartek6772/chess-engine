#include "board.hpp"
#include "board_renderer.hpp"
#include "evaluation.hpp"
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

    Search::SearchResult search_result;

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
        app.search_result = result;
    }
}

int main() {

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI | FLAG_VSYNC_HINT);
    InitWindow(1120, 800, "Visualizer");
    SetTargetFPS(60);
    rlImGuiSetup(true);

    App app;

    int cos = 0;

    while (!WindowShouldClose()) {

        bool isInteracting = IsMouseButtonDown(MOUSE_LEFT_BUTTON);

        if (isInteracting) {
            SetTargetFPS(30); // Smooth dragging
        } else {
            SetTargetFPS(10); // Idle - essentially stops the CPU hogging
        }

        update(app);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        draw(app);

        // --- ImGui ---
        rlImGuiBegin();
        ImGui::Begin("Search results");
        ImGui::Text("Hello, Engine Developer!");

        ImGui::Text("Evaluation: %d", app.search_result.score);
        ImGui::Text("Total nodes: %llu", app.search_result.stats.nodes);
        ImGui::Text("Quiescence nodes: %llu", app.search_result.stats.quiescence_nodes);
        ImGui::Text("Time [ms]: %.0ld", app.search_result.stats.time_ms);
        ImGui::Text("Nodes per second: %.0f", app.search_result.stats.nodes_per_second);
        ImGui::Text("Beta cutoffs: %lld", app.search_result.stats.beta_cutoffs);
        ImGui::End();

        ImGui::Begin("Bitbords");
        if (ImGui::Button("None")) {
            app.board_renderer.background_bitboard = BoardRenderer::BackgroundBitbord::None;
        }
        // if (ImGui::Button("White Attacks")) {
        //     app.board_renderer.background_bitboard =
        //     BoardRenderer::BackgroundBitbord::WhiteAttacks;
        // }
        // if (ImGui::Button("Black Attacks")) {
        //     app.board_renderer.background_bitboard =
        //     BoardRenderer::BackgroundBitbord::BlackAttacks;
        // }
        if (ImGui::Button("White Pawns")) {
            app.board_renderer.background_bitboard = BoardRenderer::BackgroundBitbord::WhitePawns;
        }
        if (ImGui::Button("Black Pawns")) {
            app.board_renderer.background_bitboard = BoardRenderer::BackgroundBitbord::BlackPawns;
        }
        if (ImGui::Button("Enpassant")) {
            app.board_renderer.background_bitboard = BoardRenderer::BackgroundBitbord::Enpassant;
        }
        ImGui::End();

        ImGui::Begin("Board");
        ImGui::Text("Evaluation: %d", Evaluation::evaluate(app.board));
        ImGui::Text("Cos: %d", cos);
        cos = (cos + 1) % 1000000000;
        ImGui::End();

        rlImGuiEnd();
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}