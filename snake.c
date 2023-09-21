#include "snake.h"

Snake *head = NULL;
Snake *tail = NULL;
Food *food = NULL;

int score = 0;
int main(void) {
  printf("Snake ... Starting\n");
  // Initialize window
  srand(time(0));

  food = calloc(1, sizeof(Food));
  init_snake();
  init_food();

  InitWindow(WINDOW_DIM, WINDOW_DIM, "sNaKe");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    switch (GetKeyPressed()) {
    case KEY_UP:
      if (head->dir == DOWN || head->x < 0 || head->x > WINDOW_DIM)
        break;
      head->dir = UP;
      break;
    case KEY_DOWN:
      if (head->dir == UP || head->x < 0 || head->x > WINDOW_DIM)
        break;
      head->dir = DOWN;
      break;
    case KEY_LEFT:
      if (head->dir == RIGHT || head->y < 0 || head->y > WINDOW_DIM)
        break;
      head->dir = LEFT;
      break;
    case KEY_RIGHT:
      if (head->dir == LEFT || head->y < 0 || head->y > WINDOW_DIM)
        break;
      head->dir = RIGHT;
      break;
    default:
      break;
    }
    // drawing between Begin and End
    BeginDrawing();
    ClearBackground(CLITERAL(Color){220, 220, 220, 255});
    DrawText(TextFormat("Level %d", score), 10, 10, 40, BLACK);
    move_snake();
    render_food();
    render_snake();

    if (isEaten()) {
      init_food();
      add_snake();
      score++;
    }
    if (isCollided()) {
      printf("Collision\n");
      DrawText("GAME OVER!!", 10, 10, 40, RED);
      score = 0;
      free_snake();
      init_food();
    }

    EndDrawing();
    usleep(100000);
  }
  return 0;
}

void init_snake() {
  int x = rand() % (GRID_DIM / 2) * SEGMENT_DIM;
  int y = rand() % (GRID_DIM / 2) * SEGMENT_DIM;

  Snake *new = calloc(1, sizeof(Snake));
  new->x = x;
  new->y = y;
  new->next = NULL;
  new->dir = UP;
  head = new;
  tail = new;
  return;
}

void init_food() {
  bool not_valid;
  int x, y;
  Snake *current;
  do {
    current = head;
    not_valid = true;

    x = rand() % (GRID_DIM - 1) * SEGMENT_DIM;
    y = rand() % (GRID_DIM - 1) * SEGMENT_DIM;

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

  if (head->dir == UP) {
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
      head->y = WINDOW_DIM;
    head->y -= SEGMENT_DIM;
    break;
  case DOWN:
    if (head->y == WINDOW_DIM)
      head->y = -1 * SEGMENT_DIM;
    head->y += SEGMENT_DIM;
    break;
  case LEFT:
    if (head->x == 0)
      head->x = WINDOW_DIM;
    head->x -= SEGMENT_DIM;
    break;
  case RIGHT:
    if (head->x == WINDOW_DIM)
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
    DrawRectangle(current->x, current->y, SEGMENT_DIM, SEGMENT_DIM, RED);
    current = current->next;
  }
  return;
}
void render_food() {
  DrawRectangle(food->x, food->y, SEGMENT_DIM, SEGMENT_DIM, BLUE);
  return;
}

bool isEaten() {
  if (head->x == food->x && head->y == food->y)
    return true;
  return false;
}
bool isCollided() {
  if (head->next == NULL)
    return false;
  if (head->x == tail->x && head->y == tail->y)
    return true;
  Snake *current = head->next;
  while (current != NULL) {
    if (head->x == current->x && head->y == current->y)
      return true;
    current = current->next;
  }
  return false;
}
void free_snake() {
  Snake *current = head;
  while (current != NULL) {
    Snake *s = current->next;
    free(current);
    current = s;
  }
  init_snake();
  return;
}
