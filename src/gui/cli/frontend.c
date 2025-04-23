#include "../../tetris.h"

void init_colors() {
  init_pair(1, COLOR_YELLOW, COLOR_BLACK);
  init_pair(2, COLOR_CYAN, COLOR_BLACK);
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

int center_text_x(WINDOW *win, const char *text) {
  return (getmaxx(win) - strlen(text)) / 2;
}

void mvwprint_center(WINDOW *win, int y, const char *text) {
  int width = getmaxx(win);
  int x = (width - strlen(text)) / 2;
  mvwprintw(win, y, x, "%s", text);
}

GameWindows init_windows() {
  GameWindows game_windows;

  // Центральные отступы
  int offset_y = OFFSET_Y;
  int offset_x = OFFSET_X;

  // Главное меню
  game_windows.menu_win = newwin(WIN_HEIGHT, WIN_HEIGHT, offset_y, offset_x);

  // Игровое поле  и рамка игрового поля
  game_windows.game_win =
      newwin(FIELD_HEIGHT, FIELD_WIDTH * BLOCK_WIDTH, offset_y + BORDER_PADDING,
             offset_x + BORDER_PADDING);
  game_windows.borders_win = newwin(WIN_HEIGHT, WIN_WIDTH, offset_y, offset_x);

  // Окна справа
  game_windows.next_win =
      newwin(INFO_BOX_HEIGHT, INFO_BOX_WIDTH, offset_y + 3, RIGHT_PANEL_X);
  game_windows.score_win =
      newwin(INFO_BOX_HEIGHT, INFO_BOX_WIDTH, offset_y + 8, RIGHT_PANEL_X);
  game_windows.highscore_win =
      newwin(INFO_BOX_HEIGHT, INFO_BOX_WIDTH, offset_y + 13, RIGHT_PANEL_X);

  // Окно уровня слева
  game_windows.level_win =
      newwin(INFO_BOX_HEIGHT, INFO_BOX_WIDTH, offset_y + 3, LEFT_PANEL_X);

  return game_windows;
}

void draw_field(GameInfo_t *game_info, GameWindows *game_windows) {
  wattron(game_windows->game_win, COLOR_PAIR(1));
  for (int y = 0; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      int draw_x = x * 2;
      if (game_info->field.cells[y][x] == 1) {
        mvwprintw(game_windows->game_win, y, draw_x, BLOCK_SYMBOL BLOCK_SYMBOL);
      } else {
        mvwprintw(game_windows->game_win, y, draw_x, "  ");
      }
    }
  }
  wattroff(game_windows->game_win, COLOR_PAIR(1));
}

void draw_tetromino(Tetromino *t, WINDOW *win) {
  wattron(win, COLOR_PAIR(2));
  for (int i = 0; i < 4; i++) {
    int draw_x = t->blocks[i].x * 2;
    int draw_y = t->blocks[i].y;
    mvwprintw(win, draw_y, draw_x, BLOCK_SYMBOL BLOCK_SYMBOL);
  }
  wattroff(win, COLOR_PAIR(2));
}

void draw_next_tetromino(Tetromino *t, WINDOW *win) {
  wattron(win, COLOR_PAIR(2));
  for (int i = 0; i < 4; i++) {
    int draw_x = t->blocks[i].x * 2 - 5;
    int draw_y = t->blocks[i].y + 2;
    mvwprintw(win, draw_y, draw_x, BLOCK_SYMBOL BLOCK_SYMBOL);
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
  draw_score_win(game_windows, game_info->score);
  draw_highscore_win(game_windows, game_info->high_score);
  draw_level_win(game_windows, game_info->level);
  draw_field(game_info, game_windows);
  draw_tetromino(&game_info->current_tetromino, game_windows->game_win);
  draw_next_tetromino(&game_info->next_tetromino, game_windows->next_win);

  // Подготавливаем окна для перерисовки
  wnoutrefresh(game_windows->borders_win);
  wnoutrefresh(game_windows->game_win);
  wnoutrefresh(game_windows->next_win);
  wnoutrefresh(game_windows->score_win);
  wnoutrefresh(game_windows->highscore_win);
  wnoutrefresh(game_windows->level_win);

  // Обновляем все окна
  doupdate();
}

void render_game_over(GameWindows *game_windows) {
  erase_wins(game_windows);

  wattron(game_windows->menu_win, COLOR_PAIR(1));
  mvwprint_center(game_windows->menu_win, GAME_OVER_Y, "GAME OVER");
  wattroff(game_windows->menu_win, COLOR_PAIR(1));

  refresh_wins(game_windows);
}

void refresh_wins(GameWindows *game_windows) {
  wrefresh(game_windows->game_win);
  wrefresh(game_windows->next_win);
  wrefresh(game_windows->borders_win);
  wrefresh(game_windows->menu_win);
  wrefresh(game_windows->highscore_win);
  wrefresh(game_windows->score_win);
  wrefresh(game_windows->level_win);
}

void erase_wins(GameWindows *game_windows) {
  werase(game_windows->game_win);
  werase(game_windows->next_win);
  werase(game_windows->borders_win);
  werase(game_windows->highscore_win);
  werase(game_windows->score_win);
  werase(game_windows->level_win);
}

void render_menu(WINDOW *win) {
  wattron(win, COLOR_PAIR(1));
  mvwprint_center(win, MENU_TITLE_Y, "TETRIS");

  mvwprintw(win, MENU_START_Y, MENU_X_OFFSET, "START: s");
  mvwprintw(win, MENU_PAUSE_Y, MENU_X_OFFSET, "PAUSE: p");
  mvwprintw(win, MENU_QUIT_Y, MENU_X_OFFSET, "QUIT: q");
  mvwprintw(win, MENU_MOVE_Y, MENU_X_OFFSET, "MOVE: ARROW KEYS");
  wattroff(win, COLOR_PAIR(1));

  wrefresh(win);
}

void draw_next_win(GameWindows *game_windows) {
  wattron(game_windows->next_win, COLOR_PAIR(1));
  box(game_windows->next_win, 0, 0);
  mvwprint_center(game_windows->next_win, 0, "NEXT");
  wattroff(game_windows->next_win, COLOR_PAIR(1));
}

void draw_score_win(GameWindows *game_windows, int score) {
  wattron(game_windows->score_win, COLOR_PAIR(1));
  box(game_windows->score_win, 0, 0);
  mvwprint_center(game_windows->score_win, 0, "SCORE");

  char buffer[16];
  snprintf(buffer, sizeof(buffer), "%d", score);
  mvwprint_center(game_windows->score_win, 2, buffer);

  wattroff(game_windows->score_win, COLOR_PAIR(1));
}

void draw_highscore_win(GameWindows *game_windows, int highscore) {
  wattron(game_windows->highscore_win, COLOR_PAIR(1));
  box(game_windows->highscore_win, 0, 0);
  mvwprint_center(game_windows->highscore_win, 0, "HIGHSCORE");

  char buffer[16];
  snprintf(buffer, sizeof(buffer), "%d", highscore);
  mvwprint_center(game_windows->highscore_win, 2, buffer);

  wattroff(game_windows->highscore_win, COLOR_PAIR(1));
}

void draw_level_win(GameWindows *game_windows, int level) {
  wattron(game_windows->level_win, COLOR_PAIR(1));
  box(game_windows->level_win, 0, 0);
  mvwprint_center(game_windows->level_win, 0, "LEVEL");

  char buffer[16];
  snprintf(buffer, sizeof(buffer), "%d", level);
  mvwprint_center(game_windows->level_win, 2, buffer);

  wattroff(game_windows->level_win, COLOR_PAIR(1));
}

void delete_windows(GameWindows *game_windows) {
  delwin(game_windows->borders_win);
  delwin(game_windows->game_win);
  delwin(game_windows->menu_win);
  delwin(game_windows->next_win);
  delwin(game_windows->highscore_win);
  delwin(game_windows->score_win);
  delwin(game_windows->level_win);
}