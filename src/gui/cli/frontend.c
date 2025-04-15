#include "../../tetris.h"

void init_colors() {
  init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
}

void init_ncurses(GameWindows *game_windows) {
  start_color();
  cbreak();
  keypad(stdscr, TRUE);
  keypad(game_windows->game_win, TRUE);
  keypad(game_windows->borders_win, TRUE);
  nodelay(game_windows->game_win, TRUE);  // Не блокируем wgetch() во время игры
  nodelay(game_windows->menu_win, FALSE);  // ждем ввод пользователя
  keypad(game_windows->menu_win, TRUE);
  noecho();
  curs_set(0);
  init_colors();
}

void draw_field(GameInfo_t *game_info, GameWindows *game_windows) {
  wattron(game_windows->game_win, COLOR_PAIR(1));
  for (int y = 0; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      if (game_info->field.cells[y][x] == 1) {
        mvwprintw(game_windows->game_win, y, x, "#");
      } else {
        mvwprintw(game_windows->game_win, y, x, " ");
      }
    }
  }
  wattroff(game_windows->game_win, COLOR_PAIR(1));
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

void render_all(GameWindows *game_windows, GameInfo_t *game_info) {
  werase(game_windows->game_win);
  werase(game_windows->next_win);

  draw_box(game_windows->borders_win);
  draw_next_win(game_windows);
  draw_field(game_info, game_windows);
  draw_tetromino(&game_info->current_tetromino, game_windows->game_win);
  draw_next_tetromino(&game_info->next_tetromino, game_windows->next_win);

  // Подготавливаем окна для перерисовки
  wnoutrefresh(game_windows->borders_win);
  wnoutrefresh(game_windows->game_win);
  wnoutrefresh(game_windows->next_win);
  // wnoutrefresh(game_windows->menu_win);

  // Обновляем все окна
  doupdate();
}

void render_game_over(GameWindows *game_windows) {
  werase(game_windows->game_win);
  werase(game_windows->next_win);
  werase(game_windows->borders_win);

  wattron(game_windows->menu_win, COLOR_PAIR(2));
  mvwprintw(game_windows->menu_win, FIELD_HEIGHT / 2, 2, "Game Over");
  wattroff(game_windows->menu_win, COLOR_PAIR(2));

  wrefresh(game_windows->game_win);
  wrefresh(game_windows->next_win);
  wrefresh(game_windows->borders_win);
  wrefresh(game_windows->menu_win);
}

void render_menu(WINDOW *win) {
  wattron(win, COLOR_PAIR(2));
  mvwprintw(win, 6, 3, "TETRIS");
  mvwprintw(win, 8, 0, "Start: s");
  mvwprintw(win, 9, 0, "Pause: p");
  mvwprintw(win, 10, 0, "Quit: q");
  mvwprintw(win, 11, 0, "Move: arrow keys");
  wattroff(win, COLOR_PAIR(2));

  wrefresh(win);
}

void draw_next_win(GameWindows *game_windows) {
  wattron(game_windows->next_win, COLOR_PAIR(1));
  box(game_windows->next_win, 0, 0);
  mvwprintw(game_windows->next_win, 0, 2, "Next");
  wattroff(game_windows->next_win, COLOR_PAIR(1));
}

GameWindows init_windows() {
  int offset_y = OFFSET_Y;
  int offset_x = OFFSET_X;

  GameWindows game_windows;

  game_windows.menu_win = newwin(WIN_HEIGHT, WIN_HEIGHT, offset_y, offset_x);
  game_windows.borders_win = newwin(WIN_HEIGHT, WIN_WIDTH, offset_y, offset_x);
  game_windows.game_win =
      newwin(FIELD_HEIGHT, FIELD_WIDTH, offset_y + 1, offset_x + 1);
  game_windows.next_win = newwin(6, 8, offset_y + 7, WIN_WIDTH + offset_x + 1);

  return game_windows;
}
