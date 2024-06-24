#include "game.h"
#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include <string.h>


void DrawAgent(Vector2 pos, Agent *agent) {
    float startAngle = 0;
    switch (agent->dir) {
        case DIR_UP: startAngle = 315; break;
        case DIR_LEFT: startAngle = 225; break;
        case DIR_DOWN: startAngle = 135; break;
        case DIR_RIGHT: startAngle = 45; break;
        default: break;
    }
    DrawCircleSector(
        Vector2Scale(Vector2AddValue(pos, 0.5f), CELL_SIZE),
        CELL_SIZE/2.5f,
        startAngle,
        startAngle+270,
        -1,
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

Dir RandomDir(void) {
    return GetRandomValue(0, DIR_COUNT-1);
}

Condition RandomCondition(void) {
    return GetRandomValue(0, CONDITION_COUNT-1);
}

Action RandomAction(void) {
    return GetRandomValue(0, ACTION_COUNT-1);
}

void RandomAgent(Agent *agent) {
    agent->dir = RandomDir();
    agent->health = HEALTH_MAX;
    agent->geneIndex = GetRandomValue(0, GENES_COUNT-1);
    for (size_t i = 0; i < GENES_COUNT; i++) {
        agent->genes[i].cond = RandomCondition();
        agent->genes[i].action1 = RandomAction();
        agent->genes[i].action2 = RandomAction();
        agent->genes[i].next1 = GetRandomValue(0, GENES_COUNT-1);
        agent->genes[i].next2 = GetRandomValue(0, GENES_COUNT-1);
    }
}

Vector2 ToBoardPos(Vector2 pos) {
    pos.x = (int)pos.x % BOARD_WIDTH;
    pos.y = (int)pos.y % BOARD_HEIGHT;
    return pos;
}

bool IsCellFree(Game *game, Vector2 pos) {
    if (game->agents[(int)pos.y][(int)pos.x].health <= 0 &&
        game->walls[(int)pos.y][(int)pos.x] == 0 &&
        game->foods[(int)pos.y][(int)pos.x] == 0) {
        return true;
    }
    return false;
}

Vector2 GetFrontPos(Dir dir, Vector2 pos) {
    switch (dir) {
        case DIR_UP: pos.y--; break;
        case DIR_DOWN: pos.y++; break;
        case DIR_LEFT: pos.x--; break;
        case DIR_RIGHT: pos.x++; break;
        default: break;
    }
    return ToBoardPos(pos);
}

void ExecuteAction(Game *game, Agent *agent, Vector2 pos, Action action) {
    switch (action) {
        case ACTION_MOVE: {
            Vector2 front = GetFrontPos(agent->dir, pos);
            if (IsCellFree(game, front)) {
                game->agents[(int)front.y][(int)front.x] = *agent;
                game->agents[(int)pos.y][(int)pos.x].health = 0;
            }
        } break;
        default: break;
    }
}

bool ExecuteCondition(Game *game, Agent *agent, Vector2 pos, Condition cond) {
    // TODO
    return true;
}

void UpdateAgent(Game *game, Agent *agent, Vector2 pos) {
    agent->wasUpdated = true;
    if (ExecuteCondition(game, agent, pos, agent->genes[agent->geneIndex].cond)) {
        ExecuteAction(game, agent, pos, agent->genes[agent->geneIndex].action1);
        agent->geneIndex = agent->genes[agent->geneIndex].next1;
    } else {
        ExecuteAction(game, agent, pos, agent->genes[agent->geneIndex].action2);
        agent->geneIndex = agent->genes[agent->geneIndex].next2;
    }
}

void StepGame(Game *game) {
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (!game->agents[y][x].wasUpdated && game->agents[y][x].health > 0) {
                UpdateAgent(game, &game->agents[y][x], (Vector2){x, y});
            }
        }
    }
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            game->agents[y][x].wasUpdated = false;
        }
    }
}

void InitGame(Game *game) {
    memset(game, 0, sizeof(*game));
    
    int step = 4;
    for (int y = 0; y < BOARD_HEIGHT; y += step) {
        for (int x = 0; x < BOARD_WIDTH; x += step) {
            RandomAgent(&game->agents[y][x]);
        }
    }
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

        if (IsKeyDown(KEY_SPACE)) {
            StepGame(&game);
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
