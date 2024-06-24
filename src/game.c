#include "game.h"
#include "raylib.h"
#include "raymath.h"
#include <string.h>


void DrawAgent(Vector2 pos, Agent *agent) {
    DrawCircleV(
        Vector2Scale(Vector2AddValue(pos, 0.5f), CELL_SIZE),
        CELL_SIZE/2.5f,
        RED);
}

void DrawGame(Game *game) {
    for (int y = 0; y <= BOARD_HEIGHT; y++) {
        DrawLine(0, y*CELL_SIZE, BOARD_WIDTH*CELL_SIZE, y*CELL_SIZE, GRAY);
    }
    for (int x = 0; x <= BOARD_WIDTH; x++) {
        DrawLine(x*CELL_SIZE, 0, x*CELL_SIZE, BOARD_HEIGHT*CELL_SIZE, GRAY);
    }
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (game->agents[y][x].health > 0) {
                DrawAgent((Vector2) {x, y}, &game->agents[y][x]);
            }
        }
    }
}

void InitGame(Game *game) {
    memset(game, 0, sizeof(*game));

    game->agents[0][10].health = 100;
    game->agents[1][10].health = 100;
    game->agents[10][10].health = 100;
    game->agents[0][0].health = 100;
    game->agents[3][6].health = 100;
}

int main(void) {
    Game game = {0};
    InitGame(&game);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Live");

    Camera2D camera = {0};
    camera.zoom = 1.0f;

    SetTargetFPS(60);
    
    while (!WindowShouldClose()) {
        // Update
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 delta = Vector2Scale(GetMouseDelta(), -1.0f/camera.zoom);
            camera.target = Vector2Add(camera.target, delta);
        }
        
        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
            camera.offset = GetMousePosition();
            camera.target = mouseWorldPos;

            float scaleFactor = 1.0f + (0.25*fabsf(wheel));
            if (wheel < 0) scaleFactor = 1.0f/scaleFactor;
            camera.zoom = Clamp(camera.zoom*scaleFactor, 0.125f, 64.0f);
        }

        // Draw
        BeginDrawing();
            ClearBackground(BLACK);
            
            BeginMode2D(camera);
                DrawGame(&game);
            EndMode2D();
            
            DrawFPS(0, 0);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
