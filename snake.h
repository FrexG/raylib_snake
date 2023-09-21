#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define WINDOW_DIM 800
#define SEGMENT_DIM 40
#define GRID_DIM (WINDOW_DIM / SEGMENT_DIM)

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

void init_snake();
void init_food();
void render_snake();
void render_food();
void add_snake();
void move_snake();
bool isEaten();
bool isCollided();
void free_snake();
