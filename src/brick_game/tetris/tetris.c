#include "../../tetris.h"

int main(void) {
  initscr();

  WINDOW *borders_win = newwin(WIN_HEIGHT, WIN_WIDTH, OFF_SET_Y, OFF_SET_X);
  WINDOW *game_win =
      newwin(FIELD_HEIGHT, FIELD_WIDTH, OFF_SET_Y + 1, OFF_SET_X + 1);
  WINDOW *next_win =
      newwin(6, 8, WIN_HEIGHT + OFF_SET_Y, WIN_WIDTH + OFF_SET_X);

  init_ncurses(game_win, borders_win);
  init_colors();

  int ch;

  GameInfo_t game_info = init_game_info();
  bool running = true;

  render_all(borders_win, game_win, next_win, &game_info.field,
             &game_info.tetromino);

  struct timespec last_fall, current_time;
  clock_gettime(CLOCK_MONOTONIC, &last_fall);  // Фиксируем время

  game_loop(&game_info, ch, running, last_fall, current_time, borders_win,
            game_win, next_win);

  endwin();
  return 0;
}
