#include "../../tetris.h"

int main(void) {
  initscr();

  WINDOW *borders_win = newwin(WIN_HEIGHT, WIN_WIDTH, OFF_SET_Y, OFF_SET_X);
  WINDOW *game_win =
      newwin(FIELD_HEIGHT, FIELD_WIDTH, OFF_SET_Y + 1, OFF_SET_X + 1);

  init_ncurses(game_win, borders_win);
  init_colors();

  GameField field;
  int ch;
  Tetromino t;
  bool running = true;

  init_field(&field);
  init_tetromino(&t, generate_rand_tetromino());

  render_all(borders_win, game_win, &field, &t);

  struct timespec last_fall, current_time;
  clock_gettime(CLOCK_MONOTONIC, &last_fall);  // Фиксируем время

  while (running) {
    clock_gettime(CLOCK_MONOTONIC, &current_time);  // Получаем текущее время

    // Считаем, сколько времени прошло с последнего зафиксированного момента
    double elapsed_time = get_elapsed_time(&last_fall, &current_time);

    if (elapsed_time >= FALL_DELAY) {
      move_down(&t, game_win);
      if (is_collision_below(&t, &field)) {
        stick_to_bottom(&t, &field);
        if (has_full_rows(&field)) {
          render_all(borders_win, game_win, &field, &t);

          usleep(SHIFT_DELAY);
          shift_rows(&field, game_win);
        }
        init_tetromino(&t, generate_rand_tetromino());
      }
      clock_gettime(CLOCK_MONOTONIC, &last_fall);
    }

    ch = wgetch(game_win);

    switch (ch) {
      case 'q':
        running = false;
        break;

      case KEY_DOWN:
        move_down(&t, game_win);
        if (is_collision_below(&t, &field)) {
          stick_to_bottom(&t, &field);
          if (has_full_rows(&field)) {
            render_all(borders_win, game_win, &field, &t);

            usleep(SHIFT_DELAY);
            shift_rows(&field, game_win);
          }
          init_tetromino(&t, generate_rand_tetromino());
        }
        clock_gettime(CLOCK_MONOTONIC, &last_fall);
        break;

      case KEY_RIGHT:
        move_right(&t, &field, 1);
        break;

      case KEY_LEFT:
        move_left(&t, &field, -1);
        break;

      case KEY_UP:
        rotate_tetromino(&t, &field, t.type);
        break;
    }

    if (is_game_over(&t, &field)) {
      render_game_over(game_win);
      sleep(5);
      running = false;
    }

    render_all(borders_win, game_win, &field, &t);
  }

  endwin();
  return 0;
}
