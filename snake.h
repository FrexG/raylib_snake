#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define WINDOW_W 1200
#define WINDOW_H 1000

#define SEGMENT_DIM 40
#define GRID_DIM 800

#define GRID_SIZE (GRID_DIM/ SEGMENT_DIM)

typedef enum Dir { UP, DOWN, LEFT, RIGHT } Dir;
typedef struct Food {
  int x;
  int y;
} Food;
typedef struct Snake {
  int x;
  int y;
  struct Snake *next;
  Dir dir;
} Snake;

void init_snake(int,int);
void init_food(int,int);
int get_state();
void render_snake();
void render_food();
void render_grid();
void add_snake();
void move_snake();
bool isEaten();
bool isCollided();
void free_snake();
