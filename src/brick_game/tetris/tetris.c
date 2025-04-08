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

  GameField field;
  int ch;
  Tetromino tetromino;
  Tetromino next_tetromino;
  bool running = true;

  init_field(&field);
  init_tetromino(&tetromino, generate_rand_tetromino());
  init_tetromino(&next_tetromino, generate_rand_tetromino());

  render_all(borders_win, game_win, next_win, &field, &tetromino);

  struct timespec last_fall, current_time;
  clock_gettime(CLOCK_MONOTONIC, &last_fall);  // Фиксируем время

  game_loop(field, ch, tetromino, running, last_fall, current_time, borders_win,
            game_win, next_win);

  endwin();
  return 0;
}
