#include "../../tetris.h"

int main(void)
{
  setlocale(LC_ALL, "");

  initscr();

  GameWindows game_windows = init_windows();

  init_ncurses(&game_windows);

  GameInfo_t game_info = init_game_info();

  run_game_fsm(&game_info, &game_windows);

  delete_windows(&game_windows);
  endwin();
  return 0;
}
