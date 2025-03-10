#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

typedef struct {
  int **field;
  int **next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

// void userInput(UserAction_t action, bool hold);

// GameInfo_t updateCurrentState();
void alloc_memory_field(GameInfo_t *gameInfo);
void print_field(GameInfo_t *gameInfo);


