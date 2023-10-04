#include <math.h>
/*
 * Define Environment:
 *`State Space`: [Position of the head, position of the food, direcction of the
 snake]
 *`Action Space`: The four possible directions of the sanke movnt.
 *`Reward funciton`: Positive reward for eating food , negative reward for
 hitting body.

 Q-Table: Stores the estimated Q-value for each state-action pair.
 The rows of the Q-table represent the state, columns the actions.

  Q(s, a) = Q(s, a) + alpha * (r + gamma * max_a' Q(s', a') - Q(s, a))
  - alpha: learning rate
  - r: reward
  - s: current state
  - a: current action
  - s': next state
  - a': next action
 */

// 4 head direction states and 2 food , 2 wall directions.
// distances from head to food
#define NUM_STATES (4 * 8 * 8)
#define NUM_ACTIONS 4
#define MAX_ITERATIONS 500
typedef float QTable[NUM_STATES][NUM_ACTIONS];

QTable qtable;

float epsilon = 0.9f;
const float min_eps = 0.001f;
const float eps_discount = 0.999f;
const float lr = 0.1f;

void init_qtable() {
  for (int i = 0; i < NUM_STATES; ++i) {
    for (int j = 0; j < NUM_ACTIONS; j++) {
      qtable[i][j] = 0.0f;
    }
  }
  return;
}

// update q-value for a given action-state pari
void update_q_value(int state, int action, int next_state, float reward) {
  float max_next_q_value = 0.0f;
  for (int i = 0; i < NUM_ACTIONS; ++i) {
    max_next_q_value = fmaxf(max_next_q_value, qtable[next_state][i]);
  }
  // update table
  qtable[state][action] += lr * (reward + (((1 - lr)* max_next_q_value) - qtable[state][action]));
  return;
}

// select action with the highest Q-value
int select_action(int state) {
  int best_action = 0;
  float best_q_value = qtable[state][best_action];

  //  select random actioni (explorations)
  int rand_action = rand() % NUM_ACTIONS;
  float r = (float)rand() / RAND_MAX; 
 if ( r < epsilon) {
    return rand_action;
  }

  for (int i = 0; i < NUM_ACTIONS; ++i) {
    if (qtable[state][i] > best_q_value) {
      best_q_value = qtable[state][i];
      best_action = i;
    }
  }
  return best_action;
}
