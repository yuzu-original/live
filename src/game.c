#include "game.h"
#include "raylib.h"


int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Live");
    
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("Window", 0, 0, 20, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
