#include "../../tetris.h"

int main(void) {
  initscr();

  GameWindows game_windows = init_windows();

  init_ncurses(&game_windows);

  GameInfo_t game_info = init_game_info();

  // render_all(&game_windows, &game_info);

  game_loop(&game_info, &game_windows);

  endwin();
  return 0;
}
