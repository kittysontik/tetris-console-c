
#include "tetris.h"

int main(int argc, char *argv[]) {
  GameInfo_t gameInfo;

  initscr();

  alloc_memory_field(&gameInfo);
  print_field(&gameInfo);

  refresh();
  getch();
  endwin();

  return 0;
}

void alloc_memory_field(GameInfo_t *gameInfo) {
  gameInfo->field = (int **)calloc(ROWS, sizeof(int *));

  for (int i = 0; i < ROWS; i++) {
    gameInfo->field[i] = (int *)calloc(COLS, sizeof(int));
  }
}

void print_field(GameInfo_t *gameInfo) {
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      printw("%d", gameInfo->field[i][j]);
    }
    printw("\n");
  }
}
