#include "../../tetris.h"

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

void draw_box(WINDOW *win) {
  wattron(win, COLOR_PAIR(1));
  box(win, 0, 0);
  wattroff(win, COLOR_PAIR(1));
}

void render_all(WINDOW *borders_win, WINDOW *game_win, GameField *field,
                Tetromino *t) {
  draw_box(borders_win);
  draw_field(field, game_win);
  draw_tetromino(t, game_win);

  // Подготавливаем окна для перерисовки
  wnoutrefresh(borders_win);
  wnoutrefresh(game_win);

  // Обновляем все окна
  doupdate();
}
