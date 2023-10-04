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
  init_snake();
  init_food();
  init_qtable();

  InitWindow(WINDOW_DIM, WINDOW_DIM, "sNaKe");
  SetTargetFPS(60);
  current_state = get_state();

  while (!WindowShouldClose()) {
    iterations+=1;
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
    ClearBackground(CLITERAL(Color){220, 220, 220, 255});
    DrawText(TextFormat("Level %d", score), 10, 10, 30, BLACK);
    DrawText(TextFormat("Eps %f", epsilon), 300, 10, 30, BLACK);
    DrawText(TextFormat("Episode %d", episode), 600, 10, 30, BLACK);
    DrawText(TextFormat("Iteration %d", iterations), 10,750, 30, BLACK);

    move_snake();
    render_food();
    render_snake();
    
    int new_state = get_state();

    if (isEaten()) {
      init_food();
      add_snake();
      score++;
      reward += 500;
      iterations = 0;
      update_q_value(current_state, action, new_state, reward);
    }
    if (isCollided()) {
      reward += -500;
      update_q_value(current_state, action, new_state, reward);
      reward = 0;
      score = 0;
      iterations = 0;
      episode++;

      epsilon = fmaxf(epsilon * eps_discount,min_eps);
      free_snake();
      init_food();
      }

    if(!isEaten() && iterations > MAX_ITERATIONS){
        reward +=-500;
        update_q_value(current_state, action, new_state, reward);
        reward = 0;
        score = 0;
        iterations = 0;
        episode++;
        epsilon = fmaxf(epsilon * eps_discount,min_eps);
        free_snake();
        init_food();
      }
    current_state = new_state;

    EndDrawing();
    reward--;
    update_q_value(current_state, action, new_state, reward);
    usleep(sleep_duration);
  
    }
  return 0;
}

int get_state(){
    int head_dir = head->dir;
    int food_dir;

    int w_f;
    int w_l;
    int w_r;
    // food state
    if(head->x - food->x > 0 && head->y - food->y > 0)
        food_dir = 0;

    if(head->x == food->x && head->y - food->y > 0)
      food_dir = 1;

    if(head->x - food->x < 0 && head->y - food->y > 0)
      food_dir = 2;

    if(head->x - food->x > 0 && head->y == food->y)
      food_dir = 3;

    if(head->x - food->x < 0 && head->y == food->y)
      food_dir = 4;

    if(head->x - food->x > 0 && head->y - food->y < 0)
      food_dir = 5;
    
    if(head->x == food->x && head->y - food->y < 0)
      food_dir = 6;

    if(head->x - food->x < 0 && head->y - food->y < 0)
      food_dir = 7;

    /* Wall State
     * ------------
     * Wall direction depends on the movement direction
     * of the head 
     * ------------
     * */
    if(head_dir == UP){
      w_f = (head->y == 0)?1:0;
      w_l = (head->x == 0)?1:0;
      w_r = (head->x+SEGMENT_DIM == WINDOW_DIM)?1:0;
    }

    if(head_dir == DOWN){
      w_f = (head->y+SEGMENT_DIM == WINDOW_DIM)?1:0;
      w_l = (head->x+SEGMENT_DIM == WINDOW_DIM)?1:0;
      w_r = (head->x == 0)?1:0;
    }
    
    if(head_dir == LEFT){
      w_f = (head->x == 0)?1:0;
      w_l = (head->y+SEGMENT_DIM == WINDOW_DIM)?1:0;
      w_r = (head->y == 0)?1:0;
    }
    
    if(head_dir == RIGHT){
      w_f = (head->x+SEGMENT_DIM == WINDOW_DIM)?1:0;
      w_l = (head->y == 0)?1:0;
      w_r = (head->y+SEGMENT_DIM == WINDOW_DIM)?1:0;
    }
    

    return 64*head_dir + 8*food_dir + 4*w_f + 2*w_l + w_r; 
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
  if (head->x == food->x && head->y == food->y){
    return true;
    }
  return false;
}
bool isCollided() {

  if (head->next != NULL && head->x == tail->x && head->y == tail->y)
    return true;

  if(head->dir == UP)
      if(head->y < 0)
        return true;

  if(head->dir == DOWN)
      if(head->y+SEGMENT_DIM > WINDOW_DIM)
        return true;

  if(head->dir == LEFT)
      if(head->x < 0)
        return true;

  if(head->dir == RIGHT)
      if(head->x+SEGMENT_DIM > WINDOW_DIM)
        return true;

  Snake *current = head->next;
  while (current != NULL) {
    if (head->x == current->x && head->y == current->y){
      return true;
      }
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
