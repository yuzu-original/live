#ifndef GAME_H_
#define GAME_H_

#include <stdbool.h>

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720

#define BOARD_WIDTH  100
#define BOARD_HEIGHT 100

#define CELL_SIZE 32

#define GENES_COUNT 10
#define HEALTH_MAX 100

typedef enum {
    DIR_LEFT = 0,
    DIR_RIGHT,
    DIR_UP,
    DIR_DOWN,
} Dir;

typedef enum {
    CONDITION_ALWAYS = 0,
    CONDITION_FRONT_IS_FREE,
    CONDITION_LEFT_IS_FREE,
    CONDITION_RIGHT_IS_FREE,
    CONDITION_FOOD_IN_FRONT,
    CONDITION_FOOD_ON_LEFT,
    CONDITION_FOOD_ON_RIGHT,
    CONDITION_AGENT_IN_FRONT,
    CONDITION_AGENT_ON_LEFT,
    CONDITION_AGENT_ON_RIGHT,
    CONDITION_WALL_IN_FRONT,
    CONDITION_WALL_ON_LEFT,
    CONDITION_WALL_ON_RIGHT,
    CONDITION_COUNT,
} Condition;

typedef enum {
    ACTION_DO_NOTHING = 0,
    ACTION_MOVE,
    ACTION_ATTACK,
    ACTION_EAT,
    ACTION_TURN_LEFT,
    ACTION_TURN_RIGHT,
    ACTION_COUNT,
} Action;

typedef struct {
    Condition cond;
    Action action1; // if cond is true
    int next1; // if cond is true
    Action action2; // if cond is false
    int next2; // if cond is false
} Gene;

typedef struct {
    Gene genes[GENES_COUNT];
    Dir dir;
    int health;
    int hunger;
    int geneIndex;
    bool wasUpdated;
} Agent;

typedef struct {
    Agent *agents[BOARD_HEIGHT][BOARD_WIDTH];
    int foods[BOARD_HEIGHT][BOARD_WIDTH];
    int walls[BOARD_HEIGHT][BOARD_WIDTH];
} Game;

#endif
