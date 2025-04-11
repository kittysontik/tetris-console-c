#include "../../tetris.h"

int main(void) {
  initscr();

  GameWindows game_windows = init_windows();

  init_ncurses(&game_windows);
  init_colors();

  GameInfo_t game_info = init_game_info();

  render_all(&game_windows, &game_info);

  struct timespec last_fall, current_time;
  clock_gettime(CLOCK_MONOTONIC, &last_fall);  // Фиксируем время

  game_loop(&game_info, last_fall, current_time, &game_windows);

  endwin();
  return 0;
}
