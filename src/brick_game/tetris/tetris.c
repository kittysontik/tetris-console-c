#include "../../tetris.h"

int main(void) {
  initscr();
  int offset_y = OFFSET_Y;
  int offset_x = OFFSET_X;

  WINDOW *borders_win = newwin(WIN_HEIGHT, WIN_WIDTH, offset_y, offset_x);
  WINDOW *game_win =
      newwin(FIELD_HEIGHT, FIELD_WIDTH, offset_y + 1, offset_x + 1);
  WINDOW *next_win = newwin(6, 8, offset_y + 7, WIN_WIDTH + offset_x + 1);

  init_ncurses(game_win, borders_win);
  init_colors();

  int ch;

  GameInfo_t game_info = init_game_info();
  bool running = true;

  render_all(borders_win, game_win, next_win, &game_info.field,
             &game_info.current_tetromino, &game_info.next_tetromino);

  struct timespec last_fall, current_time;
  clock_gettime(CLOCK_MONOTONIC, &last_fall);  // Фиксируем время

  game_loop(&game_info, ch, running, last_fall, current_time, borders_win,
            game_win, next_win);

  endwin();
  return 0;
}
