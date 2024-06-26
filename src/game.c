#include "game.h"
#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

char *DirToStr(Dir dir) {
    switch (dir) {
        case DIR_UP: return "DIR_UP";
        case DIR_LEFT: return "DIR_LEFT";
        case DIR_DOWN: return "DIR_DOWN";
        case DIR_RIGHT: return "DIR_RIGHT";
        default: return "";
    }
}

char *ConditionToStr(Condition cond) {
    switch (cond) {
        case CONDITION_ALWAYS: return "CONDITION_ALWAYS";
        case CONDITION_LEFT_IS_FREE: return "CONDITION_LEFT_IS_FREE";
        case CONDITION_RIGHT_IS_FREE: return "CONDITION_RIGHT_IS_FREE";
        case CONDITION_FRONT_IS_FREE: return "CONDITION_FRONT_IS_FREE";
        default: return "";
    }
}

char *ActionToStr(Action action) {
    switch (action) {
        case ACTION_MOVE: return "ACTION_MOVE";
        case ACTION_ATTACK: return "ACTION_ATTACK";
        case ACTION_DO_NOTHING: return "ACTION_DO_NOTHING";
        case ACTION_TURN_LEFT: return "ACTION_TURN_LEFT";
        case ACTION_TURN_RIGHT: return "ACTION_TURN_RIGHT";
        default: return "";
    }
}

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

void DrawAgentInfo(Agent *agent, Vector2 pos) {
    DrawText(TextFormat("Health: %d", agent->health), pos.x, pos.y, 20, WHITE);
    DrawText(TextFormat("Dir: %s", DirToStr(agent->dir)), pos.x, pos.y + 20, 20, WHITE);
    DrawText(TextFormat("Gene index: %d", agent->geneIndex), pos.x, pos.y + 40, 20, WHITE);
    for (size_t i = 0; i < GENES_COUNT; i++) {
        DrawText(TextFormat(
            "Gene %d: %s %s %d %s %d",
            i,
            ConditionToStr(agent->genes[i].cond),
            ActionToStr(agent->genes[i].action1),
            agent->genes[i].next1,
            ActionToStr(agent->genes[i].action2),
            agent->genes[i].next2
       ), pos.x, pos.y+60+i*20, 20, (agent->geneIndex == i) ? PINK : WHITE);
    }
}

void DrawGame(Game *game, Camera2D *camera) {
    // Draw borders
    // DrawRectangleLines(), doesn't work perfect
    DrawLine(0, 0, 0, CELL_SIZE*BOARD_HEIGHT, GRAY);
    DrawLine(0, 0, CELL_SIZE*BOARD_WIDTH, 0, GRAY);
    DrawLine(CELL_SIZE*BOARD_WIDTH, 0, CELL_SIZE*BOARD_WIDTH, CELL_SIZE*BOARD_HEIGHT, GRAY);
    DrawLine(0, CELL_SIZE*BOARD_HEIGHT, CELL_SIZE*BOARD_WIDTH, CELL_SIZE*BOARD_HEIGHT, GRAY);
    // Draw grid
    if (camera->zoom >= 1) {
        for (int y = 1; y < BOARD_HEIGHT; y++) {
            DrawLine(0, y*CELL_SIZE, BOARD_WIDTH*CELL_SIZE, y*CELL_SIZE, GRAY);
        }
        for (int x = 1; x < BOARD_WIDTH; x++) {
            DrawLine(x*CELL_SIZE, 0, x*CELL_SIZE, BOARD_HEIGHT*CELL_SIZE, GRAY);
        }
    }
    // Draw world
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (game->agents[y][x] != NULL) {
                DrawAgent((Vector2) {x, y}, game->agents[y][x]);
            }
        }
    }
}

Dir RandomDir(void) {
    return GetRandomValue(0, 3);
}

Condition RandomCondition(void) {
    return GetRandomValue(0, CONDITION_COUNT-1);
}

Action RandomAction(void) {
    return GetRandomValue(0, ACTION_COUNT-1);
}

Agent *RandomAgent(void) {
    Agent *a = malloc(sizeof(Agent));
    a->dir = RandomDir();
    a->health = HEALTH_MAX;
    a->geneIndex = GetRandomValue(0, GENES_COUNT-1);
    for (size_t i = 0; i < GENES_COUNT; i++) {
        a->genes[i].cond = RandomCondition();
        a->genes[i].action1 = RandomAction();
        a->genes[i].action2 = RandomAction();
        a->genes[i].next1 = GetRandomValue(0, GENES_COUNT-1);
        a->genes[i].next2 = GetRandomValue(0, GENES_COUNT-1);
    }
    return a;
}

Vector2 ToBoardPos(Vector2 pos) {
    pos.x = (int)pos.x % BOARD_WIDTH;
    if (pos.x < 0) pos.x += BOARD_WIDTH;
    pos.y = (int)pos.y % BOARD_HEIGHT;
    if (pos.y < 0) pos.y += BOARD_HEIGHT;
    return pos;
}

bool IsCellFree(Game *game, Vector2 pos) {
    if (game->agents[(int)pos.y][(int)pos.x] == NULL &&
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

Dir TurnLeft(Dir dir) {
    switch (dir) {
        case DIR_LEFT: return DIR_DOWN;
        case DIR_DOWN: return DIR_RIGHT;
        case DIR_RIGHT: return DIR_UP;
        case DIR_UP: return DIR_LEFT;
    }
    return 0;
}

Dir TurnRight(Dir dir) {
    switch (dir) {
        case DIR_LEFT: return DIR_UP;
        case DIR_DOWN: return DIR_LEFT;
        case DIR_RIGHT: return DIR_DOWN;
        case DIR_UP: return DIR_RIGHT;
    }
    return 0;
}

void ExecuteAction(Game *game, Agent *agent, Vector2 pos, Action action) {
    switch (action) {
        case ACTION_DO_NOTHING: break;
        case ACTION_MOVE: {
            Vector2 front = GetFrontPos(agent->dir, pos);
            if (IsCellFree(game, front)) {
                game->agents[(int)front.y][(int)front.x] = agent;
                game->agents[(int)pos.y][(int)pos.x] = NULL;
            }
        } break;
        case ACTION_TURN_LEFT: {
            agent->dir = TurnLeft(agent->dir);
        } break;
        case ACTION_TURN_RIGHT: {
            agent->dir = TurnRight(agent->dir);
        } break;
        case ACTION_ATTACK: {
            Vector2 front = GetFrontPos(agent->dir, pos);
            int fx = (int)front.x;
            int fy = (int)front.y;
            if (game->agents[fy][fx] != NULL) {
                game->agents[fy][fx]->health -= 10;
                if (game->agents[fy][fx]->health <= 0) {
                    free(game->agents[fy][fx]);
                    game->agents[fy][fx] = NULL;
                }
            }
        } break;
        default: break;
    }
}

bool ExecuteCondition(Game *game, Agent *agent, Vector2 pos, Condition cond) {
    // TODO make condition check
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
            if (game->agents[y][x] != NULL && !game->agents[y][x]->wasUpdated) {
                UpdateAgent(game, game->agents[y][x], (Vector2){x, y});
            }
        }
    }
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (game->agents[y][x] != NULL) {
                game->agents[y][x]->wasUpdated = false;
            }
        }
    }
}

void InitGame(Game *game) {
    memset(game, 0, sizeof(*game));
    
    int step = 3;
    for (int y = 0; y < BOARD_HEIGHT; y += step) {
        for (int x = 0; x < BOARD_WIDTH; x += step) {
            game->agents[y][x] = RandomAgent();
        }
    }
}

int main(void) {
    SetRandomSeed(time(0));
    Game game = {NULL};
    InitGame(&game);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Live");

    Camera2D camera = {0};
    camera.zoom = 1.0f;

    SetTargetFPS(60);
    
    Agent *selectedAgent = NULL;

    while (!WindowShouldClose()) {
        // Update
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 delta = Vector2Scale(GetMouseDelta(), -1.0f/camera.zoom);
            camera.target = Vector2Add(camera.target, delta);
        }
        
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
            selectedAgent = game.agents[(int)mouseWorldPos.y/CELL_SIZE][(int)mouseWorldPos.x/CELL_SIZE];
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
        } else if (IsKeyPressed(KEY_ENTER)) {
            StepGame(&game);
        }

        // Draw
        BeginDrawing();
            ClearBackground(BLACK);

            BeginMode2D(camera);
                DrawGame(&game, &camera);
            EndMode2D();
            
            DrawFPS(0, 0);
            if (selectedAgent) {
                DrawAgentInfo(selectedAgent, (Vector2){0, 20});
            }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
