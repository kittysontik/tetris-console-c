// #include "tetris.h"

// int main(int argc, char *argv[]) {
//   GameInfo_t gameInfo;

//   initscr();

//   alloc_memory_field(&gameInfo);
//   print_field(&gameInfo);

//   refresh();
//   getch();
//   endwin();

//   return 0;
// }

// void alloc_memory_field(GameInfo_t *gameInfo) {
//   gameInfo->field = (int **)calloc(20, sizeof(int *));

//   for (int i = 0; i < 20; i++) {
//     gameInfo->field[i] = (int *)calloc(10, sizeof(int));
//   }
// }

// void print_field(GameInfo_t *gameInfo) {
//   for (int i = 0; i < 20; i++) {
//     for (int j = 0; j < 10; j++) {
//       printw("%d", gameInfo->field[i][j]);
//     }
//     printw("\n");
//   }
// }
