#ifndef GAME_H_
#define GAME_H_

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720

#define BOARD_WIDTH  100
#define BOARD_HEIGHT 100

#define CELL_SIZE 32

#define CHROMOSOME_SIZE = 10;

typedef struct {
    int health;
} Agent;

typedef struct {
    Agent agents[BOARD_HEIGHT][BOARD_WIDTH];
    int foods[BOARD_HEIGHT][BOARD_WIDTH];
    int walls[BOARD_HEIGHT][BOARD_WIDTH];
} Game;

#endif
