#include "snake.h"
#include "ai.h"

Snake *head = NULL;
Snake *tail = NULL;
Food *food = NULL;

int score = 0;
int reward = 0;
int current_state;
int episode = 1;
int iterations = 0;
int sleep_duration = 100000;

int main(void) {
  printf("Snake ... Starting\n");
  // Initialize window
  srand(time(0));

  food = calloc(1, sizeof(Food));
  int grid_x = 0;
  int grid_y = (WINDOW_H / 2) - (GRID_DIM / 2);

  init_snake(grid_x, grid_y);
  init_food(grid_x, grid_y);
  init_qtable(grid_x, grid_y);

  InitWindow(WINDOW_W, WINDOW_H, "sNaKe");
  SetTargetFPS(60);

  current_state = get_state();

  while (!WindowShouldClose()) {
    iterations += 1;
    int action = select_action(current_state);
    switch (action) {
    case 0:
      if (head->dir == DOWN || head->dir == UP)
        break;
      head->dir = UP;
      break;
    case 1:
      if (head->dir == UP || head->dir == DOWN)
        break;
      head->dir = DOWN;
      break;
    case 2:
      if (head->dir == RIGHT || head->dir == LEFT)
        break;
      head->dir = LEFT;
      break;
    case 3:
      if (head->dir == LEFT || head->dir == RIGHT)
        break;
      head->dir = RIGHT;
      break;
    default:
      break;
    }
    // drawing between Begin and End
    BeginDrawing();
    ClearBackground(CLITERAL(Color){0, 0, 20, 55});
    DrawText(TextFormat("Level %d", score), 900, 90, 30, WHITE);
    DrawText(TextFormat("Eps %f", epsilon), 900, 150, 30, WHITE);
    DrawText(TextFormat("Episode %d", episode), 900, 210, 30, WHITE);
    DrawText(TextFormat("Iteration %d", iterations), 900,270, 30,WHITE);

    move_snake(grid_x, grid_y);
    render_food();
    render_snake();
    render_grid(grid_x, grid_y);

    int new_state = get_state();

    if (isEaten()) {
      init_food(grid_x, grid_y);
      add_snake();

      score++;
      reward += 500;
      iterations = 0;
      update_q_value(current_state, action, new_state, reward);
    }
    if (isCollided(grid_x, grid_y)) {
      reward += -500;
      update_q_value(current_state, action, new_state, reward);
      reward = 0;
      score = 0;
      iterations = 0;
      episode++;

      epsilon = fmaxf(epsilon * eps_discount, min_eps);
      free_snake(grid_x, grid_y);
      init_food(grid_x, grid_y);
    }

    if (!isEaten() && iterations > MAX_ITERATIONS) {
      reward += -500;
      update_q_value(current_state, action, new_state, reward);
      reward = 0;
      score = 0;
      iterations = 0;
      episode++;
      epsilon = fmaxf(epsilon * eps_discount, min_eps);
      free_snake(grid_x, grid_y);
      init_food(grid_x, grid_y);
    }
    current_state = new_state;

    EndDrawing();
    usleep(sleep_duration);
  }
  return 0;
}

int get_state() {
  int head_dir = head->dir;
  int food_dir;

  int w_f;
  int w_l;
  int w_r;

  DrawText("Snake POV", 900, 450, 30,WHITE);

  DrawRectangle(900, 500, 40, 40, CLITERAL(Color){255, 255, 255, 155});
  DrawRectangle(950, 500, 40, 40, CLITERAL(Color){255, 255, 255, 155});
  DrawRectangle(1000, 500, 40, 40, CLITERAL(Color){255, 255, 255, 155});
  DrawRectangle(900, 550, 40, 40, CLITERAL(Color){255, 255, 255, 155});
  DrawRectangle(1000, 550, 40, 40, CLITERAL(Color){255, 255, 255, 155});
  DrawRectangle(900, 600, 40, 40, CLITERAL(Color){255, 255, 255, 155});
  DrawRectangle(950, 600, 40, 40, CLITERAL(Color){255, 255, 255, 155});
  DrawRectangle(1000, 600, 40, 40, CLITERAL(Color){255, 255, 255, 155});
  // food state
  if (head->x - food->x > 0 && head->y - food->y > 0) {

    DrawRectangle(900, 500, 40, 40, CLITERAL(Color){0, 0, 255, 155});
    food_dir = 0;
  }

  if (head->x == food->x && head->y - food->y > 0) {
    DrawRectangle(950, 500, 40, 40, CLITERAL(Color){0, 0, 255, 155});
    food_dir = 1;
  }
  if (head->x - food->x < 0 && head->y - food->y > 0) {
    DrawRectangle(1000, 500, 40, 40, CLITERAL(Color){0, 0, 255, 155});
    food_dir = 2;
  }

  if (head->x - food->x > 0 && head->y == food->y) {
    DrawRectangle(900, 550, 40, 40, CLITERAL(Color){0, 0, 255, 155});
    food_dir = 3;
  }

  if (head->x - food->x < 0 && head->y == food->y) {
    DrawRectangle(1000, 550, 40, 40, CLITERAL(Color){0, 0, 255, 155});
    food_dir = 4;
  }

  if (head->x - food->x > 0 && head->y - food->y < 0) {
    DrawRectangle(900, 600, 40, 40, CLITERAL(Color){0, 0, 255, 155});
    food_dir = 5;
  }

  if (head->x == food->x && head->y - food->y < 0) {
    DrawRectangle(950, 600, 40, 40, CLITERAL(Color){0, 0, 255, 155});
    food_dir = 6;
  }

  if (head->x - food->x < 0 && head->y - food->y < 0) {
    DrawRectangle(1000, 600, 40, 40, CLITERAL(Color){0, 0, 255, 155});
    food_dir = 7;
  }

  /* Wall State
   * ------------
   * Wall direction depends on the movement direction
   * of the head
   * ------------
   * */
  if (head_dir == UP) {
    w_f = (head->y == 0) ? 1 : 0;
    w_l = (head->x == 0) ? 1 : 0;
    w_r = (head->x + SEGMENT_DIM == GRID_DIM) ? 1 : 0;
  }

  if (head_dir == DOWN) {
    w_f = (head->y + SEGMENT_DIM == GRID_DIM) ? 1 : 0;
    w_l = (head->x + SEGMENT_DIM == GRID_DIM) ? 1 : 0;
    w_r = (head->x == 0) ? 1 : 0;
  }

  if (head_dir == LEFT) {
    w_f = (head->x == 0) ? 1 : 0;
    w_l = (head->y + SEGMENT_DIM == GRID_DIM) ? 1 : 0;
    w_r = (head->y == 0) ? 1 : 0;
  }

  if (head_dir == RIGHT) {
    w_f = (head->x + SEGMENT_DIM == GRID_DIM) ? 1 : 0;
    w_l = (head->y == 0) ? 1 : 0;
    w_r = (head->y + SEGMENT_DIM == GRID_DIM) ? 1 : 0;
  }

  return 64 * head_dir + 8 * food_dir + 4 * w_f + 2 * w_l + w_r;
}

void init_snake(int grid_x, int grid_y) {

  int x = ((rand() % GRID_SIZE/4) * SEGMENT_DIM) + grid_x;
  int y = ((rand() % GRID_SIZE/4) * SEGMENT_DIM) + grid_y;

  Snake *new = calloc(1, sizeof(Snake));
  new->x = x;
  new->y = y;
  new->next = NULL;
  new->dir = UP;
  head = new;
  tail = new;
  return;
}

void init_food(int grid_x, int grid_y) {
  bool not_valid;
  int x, y;
  Snake *current;
  do {
    current = head;
    not_valid = true;

    x = (rand() % (GRID_SIZE - 1) * SEGMENT_DIM) + grid_x;
    y = (rand() % (GRID_SIZE - 1) * SEGMENT_DIM) + grid_y;

    while (current != NULL) {
      if (x == current->x && y == current->y)
        break;

      if (current->next == NULL)
        not_valid = false;
      current = current->next;
    }

  } while (not_valid);

  food->x = x;
  food->y = y;
}

void add_snake() {
  /* Add segment at the tail */
  Snake *new = calloc(1, sizeof(Snake));

  if (tail->dir == UP) {
    new->x = tail->x;
    new->y = tail->y + SEGMENT_DIM;
    new->next = NULL;
    new->dir = tail->dir;
  }

  else if (tail->dir == DOWN) {
    new->x = tail->x;
    new->y = tail->y - SEGMENT_DIM;
    new->next = NULL;
    new->dir = tail->dir;
  }

  else if (tail->dir == LEFT) {
    new->x = tail->x + SEGMENT_DIM;
    new->y = tail->y;
    new->next = NULL;
    new->dir = tail->dir;
  }

  else {
    new->x = tail->x - SEGMENT_DIM;
    new->y = tail->y;
    new->next = NULL;
    new->dir = tail->dir;
  }

  tail->next = new;
  tail = new;
  return;
}

void move_snake() {
  int head_x = head->x;
  int head_y = head->y;

  switch (head->dir) {
  case UP:
    if (head->y == 0)
      head->y = GRID_DIM;
    head->y -= SEGMENT_DIM;
    break;
  case DOWN:
    if (head->y == GRID_DIM)
      head->y = -1 * SEGMENT_DIM;
    head->y += SEGMENT_DIM;
    break;
  case LEFT:
    if (head->x == 0)
      head->x = GRID_DIM;
    head->x -= SEGMENT_DIM;
    break;
  case RIGHT:
    if (head->x == GRID_DIM)
      head->x = -1 * SEGMENT_DIM;
    head->x += SEGMENT_DIM;
    break;
  }

  Snake *current = head->next;

  while (current != NULL) {
    int current_x = current->x;
    int current_y = current->y;

    current->x = head_x;
    current->y = head_y;

    current = current->next;
    head_x = current_x;
    head_y = current_y;
  }
  return;
}

void render_snake() {
  Snake *current = head;
  while (current != NULL) {
    DrawRectangle(current->x, current->y, SEGMENT_DIM, SEGMENT_DIM, GREEN);
    current = current->next;
  }
  return;
}
void render_food() {
  DrawRectangle(food->x, food->y, SEGMENT_DIM, SEGMENT_DIM, RED);
  return;
}

void render_grid(int grid_x, int grid_y) {
  // Draw horizontal grids
  DrawRectangle(grid_x, grid_y, GRID_DIM, 1, WHITE);

  DrawRectangle(grid_x + GRID_DIM, grid_y, 1, GRID_DIM, WHITE);

  DrawRectangle(grid_x, grid_y + GRID_DIM, GRID_DIM, 1, WHITE);

  return;
}

bool isEaten() {
  if (head->x == food->x && head->y == food->y) {
    return true;
  }
  return false;
}
bool isCollided(int grid_x, int grid_y) {

  if (head->next != NULL && head->x == tail->x && head->y == tail->y)
    return true;

  if (head->dir == UP)
    if (head->y <= grid_y)
      return true;

  if (head->dir == DOWN)
    if (head->y + SEGMENT_DIM >= GRID_DIM + grid_y)
      return true;

  if (head->dir == LEFT)
    if (head->x <= grid_x)
      return true;

  if (head->dir == RIGHT)
    if (head->x + SEGMENT_DIM >= GRID_DIM + grid_x)
      return true;

  Snake *current = head->next;
  while (current != NULL) {
    if (head->x == current->x && head->y == current->y) {
      return true;
    }
    current = current->next;
  }
  return false;
}
void free_snake(int grid_x, int grid_y) {
  Snake *current = head;
  while (current != NULL) {
    Snake *s = current->next;
    free(current);
    current = s;
  }
  init_snake(grid_x, grid_y);
  return;
}
