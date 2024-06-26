#include "game.h"
#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
        case CONDITION_FOOD_IN_FRONT: return "CONDITION_FOOD_IN_FRONT";
        case CONDITION_FOOD_ON_LEFT: return "CONDITION_FOOD_ON_LEFT";
        case CONDITION_FOOD_ON_RIGHT: return "CONDITION_FOOD_ON_RIGHT";
        case CONDITION_AGENT_IN_FRONT: return "CONDITION_AGENT_IN_FRONT";
        case CONDITION_AGENT_ON_LEFT: return "CONDITION_AGENT_ON_LEFT";
        case CONDITION_AGENT_ON_RIGHT: return "CONDITION_AGENT_ON_RIGHT";
        case CONDITION_WALL_IN_FRONT: return "CONDITION_WALL_IN_FRONT";
        case CONDITION_WALL_ON_LEFT: return "CONDITION_WALL_ON_LEFT";
        case CONDITION_WALL_ON_RIGHT: return "CONDITION_WALL_ON_RIGHT";
        default: return "";
    }
}

char *ActionToStr(Action action) {
    switch (action) {
        case ACTION_MOVE: return "ACTION_MOVE";
        case ACTION_ATTACK: return "ACTION_ATTACK";
        case ACTION_EAT: return "ACTION_EAT";
        case ACTION_DO_NOTHING: return "ACTION_DO_NOTHING";
        case ACTION_TURN_LEFT: return "ACTION_TURN_LEFT";
        case ACTION_TURN_RIGHT: return "ACTION_TURN_RIGHT";
        case ACTION_REPRODUCE: return "ACTION_REPRODUCE";
        default: return "";
    }
}

void DrawWall(Vector2 pos) {
    DrawRectanglePro((Rectangle){pos.x*CELL_SIZE, pos.y*CELL_SIZE, CELL_SIZE, CELL_SIZE}, (Vector2){-0.5f, -0.5f}, 0, GRAY);
}

void DrawFood(Vector2 pos) {
    DrawCircleV(Vector2Scale(Vector2AddValue(pos, 0.5), CELL_SIZE), CELL_SIZE/4.0f, ORANGE);
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
    DrawText(TextFormat("Hunger: %d", agent->hunger), pos.x, pos.y + 20, 20, WHITE);
    DrawText(TextFormat("Dir: %s", DirToStr(agent->dir)), pos.x, pos.y + 40, 20, WHITE);
    DrawText(TextFormat("Gene index: %d", agent->geneIndex), pos.x, pos.y + 60, 20, WHITE);
    for (size_t i = 0; i < GENES_COUNT; i++) {
        DrawText(TextFormat(
            "Gene %d: %s %s %d %s %d",
            i,
            ConditionToStr(agent->genes[i].cond),
            ActionToStr(agent->genes[i].action1),
            agent->genes[i].next1,
            ActionToStr(agent->genes[i].action2),
            agent->genes[i].next2
       ), pos.x, pos.y+80+i*20, 20, (agent->geneIndex == i) ? PINK : WHITE);
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
                DrawAgent((Vector2){x, y}, game->agents[y][x]);
            } else if (game->walls[y][x] != 0) {
                DrawWall((Vector2){x, y});
            } else if (game->foods[y][x] != 0) {
                DrawFood((Vector2){x, y});
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
    a->hunger = 100;
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

Dir GetReverseDir(Dir dir) {
    switch (dir) {
        case DIR_LEFT: return DIR_RIGHT;
        case DIR_DOWN: return DIR_UP;
        case DIR_RIGHT: return DIR_LEFT;
        case DIR_UP: return DIR_DOWN;
        default: return 0;
    }
}

void KillAgent(Game *game, Agent *agent, Vector2 pos) {
    // shift by 1 element to right
    memmove(&game->bestGenes[1], &game->bestGenes[0], (BEST_GENES_COUNT-1)*GENES_COUNT*sizeof(Gene));
    // set first element to curent genes
    memcpy(&game->bestGenes[0], &agent->genes, GENES_COUNT*sizeof(Gene));
    
    game->bestGenesCount = Clamp(game->bestGenesCount+1, 0, BEST_GENES_COUNT);

    game->foods[(int)pos.y][(int)pos.x] = (agent->hunger > 10) ? agent->hunger : 10;
    
    free(agent);
    game->agents[(int)pos.y][(int)pos.x] = NULL;
}

Agent *ReproduceAgent(Agent *parent) {
    Agent *a = malloc(sizeof(Agent));
    a->dir = GetReverseDir(parent->dir);
    a->health = HEALTH_MAX;
    a->hunger = parent->hunger/2;
    parent->hunger /= 2;
    a->health = parent->health;
    a->geneIndex = GetRandomValue(0, GENES_COUNT-1);
    for (size_t i = 0; i < GENES_COUNT; i++) {
        a->genes[i].cond = parent->genes[i].cond;
        a->genes[i].action1 = parent->genes[i].action1;
        a->genes[i].action2 = parent->genes[i].action2;
        a->genes[i].next1 = parent->genes[i].next1;
        a->genes[i].next2 = parent->genes[i].next2;
        if (GetRandomValue(0, 100) <= 10) {
            int m = GetRandomValue(0, 4); // cond, action1, action2, ...
            switch (m) {
                case 0: a->genes[i].cond = RandomCondition(); break;
                case 1: a->genes[i].action1 = RandomAction(); break;
                case 2: a->genes[i].action2 = RandomAction(); break;
                case 3: a->genes[i].next1 = GetRandomValue(0, GENES_COUNT-1); break;
                case 4: a->genes[i].next2 = GetRandomValue(0, GENES_COUNT-1); break;
            }
        }
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

Vector2 GetBackPos(Dir dir, Vector2 pos) {
    switch (dir) {
        case DIR_UP: pos.y++; break;
        case DIR_DOWN: pos.y--; break;
        case DIR_LEFT: pos.x++; break;
        case DIR_RIGHT: pos.x--; break;
        default: break;
    }
    return ToBoardPos(pos);
}

Vector2 GetLeftPos(Dir dir, Vector2 pos) {
    switch (dir) {
        case DIR_UP: pos.x--; break;
        case DIR_DOWN: pos.x++; break;
        case DIR_LEFT: pos.y++; break;
        case DIR_RIGHT: pos.y--; break;
        default: break;
    }
    return ToBoardPos(pos);
}

Vector2 GetRightPos(Dir dir, Vector2 pos) {
    switch (dir) {
        case DIR_UP: pos.x++; break;
        case DIR_DOWN: pos.x--; break;
        case DIR_LEFT: pos.y--; break;
        case DIR_RIGHT: pos.y++; break;
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
                    KillAgent(game, agent, pos);
                }
            }
        } break;
        case ACTION_EAT: {
            Vector2 front = GetFrontPos(agent->dir, pos);
            int fx = (int)front.x;
            int fy = (int)front.y;
            if (game->foods[fy][fx] != 0) {
                agent->hunger += game->foods[fy][fx];
                game->foods[fy][fx] = 0;
            }
        } break;
        case ACTION_REPRODUCE: {
            Vector2 back = GetBackPos(agent->dir, pos);
            int bx = (int)back.x;
            int by = (int)back.y;
            if (IsCellFree(game, back)) {
                game->agents[by][bx] = ReproduceAgent(agent);
            }
        } break;
        default: break;
    }
}

bool ExecuteCondition(Game *game, Agent *agent, Vector2 pos, Condition cond) {
    switch (cond) {
        case CONDITION_ALWAYS: return true;
        case CONDITION_FRONT_IS_FREE: return IsCellFree(game, GetFrontPos(agent->dir, pos));
        case CONDITION_LEFT_IS_FREE: return IsCellFree(game, GetLeftPos(agent->dir, pos));
        case CONDITION_RIGHT_IS_FREE: return IsCellFree(game, GetRightPos(agent->dir, pos));
        case CONDITION_FOOD_IN_FRONT: {
            Vector2 front = GetFrontPos(agent->dir, pos);
            return game->foods[(int)front.y][(int)front.x] != 0;
        };
        case CONDITION_FOOD_ON_LEFT: {
            Vector2 left = GetLeftPos(agent->dir, pos);
            return game->foods[(int)left.y][(int)left.x] != 0;
        };
        case CONDITION_FOOD_ON_RIGHT: {
            Vector2 right = GetRightPos(agent->dir, pos);
            return game->foods[(int)right.y][(int)right.x] != 0;
        };
        case CONDITION_AGENT_IN_FRONT: {
            Vector2 front = GetFrontPos(agent->dir, pos);
            return game->agents[(int)front.y][(int)front.x] != NULL;
        };
        case CONDITION_AGENT_ON_LEFT: {
            Vector2 left = GetLeftPos(agent->dir, pos);
            return game->agents[(int)left.y][(int)left.x] != NULL;
        };
        case CONDITION_AGENT_ON_RIGHT: {
            Vector2 right = GetRightPos(agent->dir, pos);
            return game->agents[(int)right.y][(int)right.x] != NULL;
        };
        case CONDITION_WALL_IN_FRONT: {
            Vector2 front = GetFrontPos(agent->dir, pos);
            return game->walls[(int)front.y][(int)front.x] != 0;
        };
        case CONDITION_WALL_ON_LEFT: {
            Vector2 left = GetLeftPos(agent->dir, pos);
            return game->walls[(int)left.y][(int)left.x] != 0;
        };
        case CONDITION_WALL_ON_RIGHT: {
            Vector2 right = GetRightPos(agent->dir, pos);
            return game->walls[(int)right.y][(int)right.x] != 0;
        };
        default: return true;
    }
}

void UpdateAgent(Game *game, Agent *agent, Vector2 pos) {
    agent->wasUpdated = true;
    agent->hunger -= 5;
    if (agent->hunger < 0) {
        agent->hunger = 0;
        agent->health -= 10;
        if (agent->health <= 0) {
            KillAgent(game, agent, pos);
            return;
        }
    }
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
    game->allDie = true;
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (game->agents[y][x] != NULL) {
                game->agents[y][x]->wasUpdated = false;
                game->allDie = false;
            }
        }
    }
}

void CreateWallsAndFoods(Game *game) {
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (IsCellFree(game, (Vector2){x, y})) {
                if (GetRandomValue(0, 100) <= 1) {
                    game->walls[y][x] = 1;
                } else if (GetRandomValue(0, 100) <= 30) {
                    game->foods[y][x] = 50;
                }
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
    
    CreateWallsAndFoods(game);
}

Agent *AgentFromGenes(Gene genes[GENES_COUNT]) {
    Agent *a = malloc(sizeof(Agent));
    a->dir = RandomDir();
    a->health = HEALTH_MAX;
    a->hunger = 100;
    a->geneIndex = GetRandomValue(0, GENES_COUNT - 1);
    for (size_t i = 0; i < GENES_COUNT; i++) {
        a->genes[i] = genes[i];
    }
    return a;
}

void ReinitGame(Game *game) {
    Gene bestGenes[BEST_GENES_COUNT][GENES_COUNT];
    memcpy(bestGenes, game->bestGenes, BEST_GENES_COUNT*GENES_COUNT*sizeof(Gene));
    int bestGenesCount = game->bestGenesCount;

    memset(game, 0, sizeof(*game));

    int step = 3;
    for (int y = 0; y < BOARD_HEIGHT; y += step) {
        for (int x = 0; x < BOARD_WIDTH; x += step) {
            if (GetRandomValue(0, 100) <= 70) {
                game->agents[y][x] = AgentFromGenes(bestGenes[GetRandomValue(0, bestGenesCount-1)]);
            } else {
                game->agents[y][x] = RandomAgent();
            }
        }
    } 

    CreateWallsAndFoods(game);
}

int main(void) {
    SetRandomSeed(time(0));
    Game game = {0};
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
            if (game.allDie) ReinitGame(&game);
            StepGame(&game);
        } else if (IsKeyPressed(KEY_ENTER)) {
            if (game.allDie) ReinitGame(&game);
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
