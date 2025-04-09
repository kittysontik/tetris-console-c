#include "../../tetris.h"

void init_ncurses(WINDOW *game_win, WINDOW *borders_win) {
  start_color();
  cbreak();
  keypad(stdscr, TRUE);
  keypad(game_win, TRUE);
  keypad(borders_win, TRUE);
  nodelay(game_win, TRUE);  // Не блокируем getch()
  noecho();
  curs_set(0);
}
void init_colors() {
  init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
}

void draw_field(GameField *field, WINDOW *win) {
  wattron(win, COLOR_PAIR(1));
  for (int y = 0; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      if (field->cells[y][x] == 1) {
        mvwprintw(win, y, x, "#");
      } else {
        mvwprintw(win, y, x, ".");
      }
    }
  }
  wattroff(win, COLOR_PAIR(1));
}

void draw_tetromino(Tetromino *t, WINDOW *win) {
  wattron(win, COLOR_PAIR(2));
  for (int i = 0; i < 4; i++) {
    mvwprintw(win, t->blocks[i].y, t->blocks[i].x, "#");
  }
  wattroff(win, COLOR_PAIR(2));
}

void draw_next_tetromino(Tetromino *t, WINDOW *win) {
  wattron(win, COLOR_PAIR(2));
  for (int i = 0; i < 4; i++) {
    mvwprintw(win, t->blocks[i].y + 2, t->blocks[i].x - 1, "#");
  }
  wattroff(win, COLOR_PAIR(2));
}

void draw_box(WINDOW *win) {
  wattron(win, COLOR_PAIR(1));
  box(win, 0, 0);
  wattroff(win, COLOR_PAIR(1));
}

void render_all(WINDOW *borders_win, WINDOW *game_win, WINDOW *next_win,
                GameField *field, Tetromino *t, Tetromino *next) {
  werase(game_win);
  werase(next_win);

  draw_box(borders_win);
  draw_next_win(next_win);
  draw_field(field, game_win);
  draw_tetromino(t, game_win);
  draw_next_tetromino(next, next_win);

  // Подготавливаем окна для перерисовки
  wnoutrefresh(borders_win);
  wnoutrefresh(game_win);
  wnoutrefresh(next_win);

  // Обновляем все окна
  doupdate();
}

void render_game_over(WINDOW *game_win, WINDOW *next_win) {
  wclear(game_win);
  wclear(next_win);

  mvwprintw(game_win, FIELD_HEIGHT / 2, 1, "Game Over");
  wrefresh(game_win);
  wrefresh(next_win);

  wgetch(game_win);
}

void draw_next_win(WINDOW *win) {
  wattron(win, COLOR_PAIR(1));
  box(win, 0, 0);
  mvwprintw(win, 0, 2, "Next");
  wattroff(win, COLOR_PAIR(1));
}
