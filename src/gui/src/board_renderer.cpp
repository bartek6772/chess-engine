#include "board_renderer.hpp"
#include "move_generator.hpp"
#include "pieces.hpp"

Rectangle getSource(int piece) {
    int col = 6 - Pieces::pieceType(piece);
    int row = Pieces::isWhite(piece) ? 0 : 1;
    constexpr float size = 2000.0 / 6;
    return { float(col * size), float(row * size), size, size };
}

BoardRenderer::BoardRenderer(int pos_x, int pos_y, bool interactable)
    : interactable(interactable), pos_x(pos_x), pos_y(pos_y) {

    const char* appDir = GetApplicationDirectory();
    const char* piecesPath = TextFormat("%sassets/pieces.png", appDir);
    pieces_texture = LoadTexture(piecesPath);
}

BoardRenderer::~BoardRenderer() {
    // UnloadTexture(pieces_texture);
}

void BoardRenderer::drawBoard(const Board& board) {

    unsigned long long bitboard = 0;
    switch (background_bitboard) {
        case BackgroundBitbord::None: break;
        // case BackgroundBitbord::WhiteAttacks: bitboard = move_gen.getWhiteAttacks(); break;
        // case BackgroundBitbord::BlackAttacks: bitboard = move_gen.getBlackAttacks(); break;
        case BackgroundBitbord::WhitePawns: bitboard = board.bitboards[Pieces::WhitePawn]; break;
        case BackgroundBitbord::BlackPawns: bitboard = board.bitboards[Pieces::BlackPawn]; break;
        case BackgroundBitbord::Enpassant:
            if (board.enpassant_square != -1) bitboard = (1ULL << board.enpassant_square);
            break;
    }

    for (int row = 7; row >= 0; row--) {
        for (int col = 0; col < 8; col++) {
            Color color = (row + col) & 1 ? white_square : black_square;

            Rectangle destination = {
                pos_x + float(col) * square_size,
                pos_y + float(7 - row) * square_size,
                square_size,
                square_size,
            };

            DrawRectanglePro(destination, { 0, 0 }, 0, color);

            int square = row * 8 + col;
            if ((bitboard & (1ULL << square)) != 0) {
                DrawRectanglePro(destination, { 0, 0 }, 0, bitbord_overlay);
            }

            if (board.squares[square] != Pieces::None && drag_start != square) {
                Rectangle source = getSource(board.squares[square]);
                DrawTexturePro(pieces_texture, source, destination, { 0, 0 }, 0, WHITE);
            }
        }
    }
}

void BoardRenderer::drawMoves() {
    if (drag_start == -1) return;

    for (int square : move_map[drag_start]) {
        int row = square / 8;
        int col = square % 8;
        float scale = 0.5F;

        Color circle_color = BLACK;
        circle_color.a = 100;
        DrawCircle(pos_x + col * square_size + square_size / 2,
            pos_y + (7 - row) * square_size + square_size / 2, square_size * scale / 2,
            circle_color);
    }
}

void BoardRenderer::drawDraggedPiece() {
    if (drag_start == -1) return;

    Rectangle dest = {
        pos_x + (float)GetMouseX() - square_size / 2.0F,
        pos_y + (float)GetMouseY() - square_size / 2.0F,
        square_size,
        square_size,
    };
    Rectangle source = getSource(dragged_piece);
    DrawTexturePro(pieces_texture, source, dest, { 0, 0 }, 0, WHITE);
}

void BoardRenderer::draw(const Board& board) {
    drawBoard(board);
    drawMoves();
    drawDraggedPiece();
}

void BoardRenderer::makeMoveMap(Board& board) {
    available_moves = MoveGenerator::generateLegalMoves(board);

    for (auto& vec : move_map) {
        vec.clear();
    }

    for (const Move& move : available_moves) {
        move_map[move.from].push_back(move.to);
    }
}

std::optional<Move> BoardRenderer::update(Board& board) {

    if (!interactable) return std::nullopt;

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && dragged_piece == 0) {

        if (GetMouseX() < pos_x || GetMouseX() > pos_x + square_size * 8) return std::nullopt;
        if (GetMouseY() < pos_y || GetMouseY() > pos_y + square_size * 8) return std::nullopt;

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

        drag_start = -1;
        dragged_piece = 0;

        if (GetMouseX() < pos_x || GetMouseX() > pos_x + square_size * 8) return std::nullopt;
        if (GetMouseY() < pos_y || GetMouseY() > pos_y + square_size * 8) return std::nullopt;

        if (!final_move.isNull()) {
            return final_move;
        }
    }

    return std::nullopt;
}