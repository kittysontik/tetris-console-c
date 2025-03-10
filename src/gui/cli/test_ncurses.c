#include "tetris.h"

typedef struct win_border_struct {
  chtype ls, rs, ts, bs, tl, tr, bl, br;  // s — side, t — top, b — bottom
} WIN_BORDER;

typedef struct WIN_struct {
  int startx, starty;
  int height, width;
  WIN_BORDER border;
} WIN;

typedef struct {
  int x[4];
  int y[4];
} Tetromino;

void init_tetromino(Tetromino *t, int type) {
  if (type == 0) {  // квадрат
    t->x[0] = 2;
    t->y[0] = 1;
    t->x[1] = 3;
    t->y[1] = 1;
    t->x[2] = 2;
    t->y[2] = 2;
    t->x[3] = 3;
    t->y[3] = 2;
  }  // массивы координат для фигуры квадрат
}

void draw_tetromino(WIN *win, Tetromino *t) {
  for (int i = 0; i < 4; i++) {
    mvaddch(win->starty + t->y[i], win->startx + t->x[i], '#');
  }
  refresh();
}

void erase_tetromino(WIN *win, Tetromino *t) {
  for (int i = 0; i < 4; i++) {
    mvaddch(win->starty + t->y[i], win->startx + t->x[i], '.');
  }
}
void move_tetromino(WIN *win, Tetromino *t, int dx, int dy) {
  erase_tetromino(win, t);

  for (int i = 0; i < 4; i++) {
    t->x[i] += dx;
    t->y[i] += dy;
  }
  draw_tetromino(win, t);
}

void init_win_params(WIN *p_win);
void print_win_params(WIN *p_win);
void create_box(WIN *win, bool flag);

int main(void) {
  WIN win;
  Tetromino tet;
  int ch;

  initscr();  // allocates memory for present window which is called stdscr
  start_color();
  cbreak();
  keypad(stdscr, TRUE);  // for F1 key
  noecho();
  init_pair(1, COLOR_CYAN, COLOR_BLACK);

  init_win_params(&win);

  attron(COLOR_PAIR(1));
  printw("Press F1 to exit. Use arrow keys to move the square");
  refresh();
  attroff(COLOR_PAIR(1));

  create_box(&win, TRUE);

  init_tetromino(&tet, 0);
  draw_tetromino(&win, &tet);

  while ((ch = getch()) != KEY_F(1)) {  // Пока не нажата клавиша F1
    if (ch == KEY_UP) {
      move_tetromino(&win, &tet, 0, -1);  // Двигаем вверх
    } else if (ch == KEY_DOWN) {
      move_tetromino(&win, &tet, 0, 1);  // Двигаем вниз
    } else if (ch == KEY_LEFT) {
      move_tetromino(&win, &tet, -1, 0);  // Двигаем влево
    } else if (ch == KEY_RIGHT) {
      move_tetromino(&win, &tet, 1, 0);  // Двигаем вправо
    }
  }

  endwin();
  return 0;
}

void init_win_params(WIN *p_win) {
  p_win->height = 22;
  p_win->width = 12;
  p_win->starty = 1;
  p_win->startx = 1;

  p_win->border.ls = ACS_VLINE;
  p_win->border.rs = ACS_VLINE;
  p_win->border.ts = ACS_HLINE;
  p_win->border.bs = ACS_HLINE;
  p_win->border.tl = ACS_ULCORNER;
  p_win->border.tr = ACS_URCORNER;
  p_win->border.bl = ACS_LLCORNER;
  p_win->border.br = ACS_LRCORNER;
}

void print_win_params(WIN *p_win) {
#ifdef _DEBUG
  mvprintw(25, 0, "%d %d %d %d", p_win->startx, p_win->starty, p_win->width,
           p_win->height);
  refresh();
#else
  (void)p_win;
#endif
}

void create_box(WIN *p_win, bool flag) {
  int i, j;
  int x, y, w, h;

  x = p_win->startx;
  y = p_win->starty;
  w = p_win->width;
  h = p_win->height;

  if (flag == TRUE) {
    // field corners coordinates
    mvaddch(y, x, p_win->border.tl);
    mvaddch(y, x + w - 1, p_win->border.tr);
    mvaddch(y + h - 1, x, p_win->border.bl);
    mvaddch(y + h - 1, x + w - 1, p_win->border.br);
    // field borders coordinates
    mvhline(y, x + 1, p_win->border.ts, w - 2);
    mvhline(y + h - 1, x + 1, p_win->border.bs, w - 2);
    mvvline(y + 1, x, p_win->border.ls, h - 2);
    mvvline(y + 1, x + w - 1, p_win->border.rs, h - 2);

    for (j = y + 1; j < h; j++) {
      for (i = x + 1; i < w; i++) {
        mvaddch(j, i, '.');
      }
    }

  } else {
    for (j = y; j <= y + h; ++j) {
      for (i = x; i <= x + w; ++i) {
        mvaddch(j, i, ' ');
      }
    }
  }

  refresh();
}
