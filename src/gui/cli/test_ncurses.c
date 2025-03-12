#include <ncurses.h>

// #include "tetris.h"

typedef struct {
  chtype leftSide, rightSide, topSide, bottomSide, topLeftCorner,
      topRightCorner, bottomLeftCorner, bottomRightCorner;
} Border;

typedef struct {
  int x, y;
  int width, height;
  Border border;
} GameField;

typedef struct {
  int x, y;
} Block;

typedef struct {
  Block blocks[4];
} Tetromino;

typedef enum {
  TETROMINO_I,  // линия
  TETROMINO_L,  // Г-образная
  TETROMINO_J,  // обратная Г-образная
  TETROMINO_O,  // квадрат
  TETROMINO_S,  // s-образная
  TETROMINO_T,  // т-образная
  TETROMINO_Z,  // z-образная
  TETROMINO_COUNT
} TetrominoType;

static const Tetromino tetrominoes[TETROMINO_COUNT] = {
    {{{1, 1}, {2, 1}, {3, 1}, {4, 1}}},  // I
    {{{1, 2}, {1, 1}, {2, 1}, {3, 1}}},  // L
    {{{3, 2}, {3, 1}, {2, 1}, {1, 1}}},  // J
    {{{1, 1}, {2, 1}, {2, 2}, {2, 1}}},  // O square
    {{{1, 1}, {2, 1}, {2, 2}, {3, 2}}},  // S
    {{{1, 1}, {2, 1}, {2, 2}, {3, 1}}},  // T
    {{{1, 2}, {2, 2}, {2, 1}, {3, 1}}}   // Z
};

void init_tetromino(Tetromino *t, TetrominoType type) {
  Tetromino selectedTetro = tetrominoes[type];

  for (int i = 0; i < 4; i++) {
    t->blocks[i] = selectedTetro.blocks[i];
  }
}

void init_field(GameField *field) {
  field->height = 22;
  field->width = 12;
  field->x = 1;
  field->y = 1;

  field->border.leftSide = ACS_VLINE;
  field->border.rightSide = ACS_VLINE;
  field->border.topSide = ACS_HLINE;
  field->border.bottomSide = ACS_HLINE;

  field->border.topLeftCorner = ACS_ULCORNER;
  field->border.topRightCorner = ACS_URCORNER;
  field->border.bottomLeftCorner = ACS_LLCORNER;
  field->border.bottomRightCorner = ACS_LRCORNER;
}

void draw_corners(GameField *field) {
  mvaddch(field->y, field->x, field->border.topLeftCorner);
  mvaddch(field->y, (field->x + field->width - 1),
          field->border.topRightCorner);
  mvaddch((field->y + field->height - 1), field->x,
          field->border.bottomLeftCorner);
  mvaddch((field->y + field->height - 1), (field->x + field->width - 1),
          field->border.bottomRightCorner);
}

void draw_borders(GameField *field) {
  mvhline(field->y, field->x + 1, field->border.topSide, field->width - 2);
  mvhline(field->y + field->height - 1, field->x + 1, field->border.bottomSide,
          field->width - 2);

  mvvline(field->y + 1, field->x, field->border.leftSide, field->height - 2);
  mvvline(field->y + 1, field->x + field->width - 1, field->border.rightSide,
          field->height - 2);
}

void fill_field(GameField *field) {
  for (int j = field->y + 1; j < field->y + field->height - 1; j++) {
    for (int i = field->x + 1; i < field->x + field->width - 1; i++) {
      mvaddch(j, i, '.');
    }
  }
}

void clear_field(GameField *field) {
  for (int j = field->y; j < field->y + field->height; j++) {
    for (int i = field->x; i < field->x + field->width; i++) {
      mvaddch(j, i, ' ');
    }
  }
}

void draw_field(GameField *field, bool draw) {
  if (draw) {
    draw_corners(field);
    draw_borders(field);
    fill_field(field);
  } else {
    clear_field(field);
  }
  refresh();
}

int main(void) {
  GameField field;
  int ch;

  initscr();
  start_color();
  cbreak();
  keypad(stdscr, TRUE);
  noecho();
  refresh();

  init_pair(1, COLOR_CYAN, COLOR_BLACK);

  init_field(&field);

  attron(COLOR_PAIR(1));
  printw("Press F1 to exit. Use arrow keys to move the square");
  refresh();

  attroff(COLOR_PAIR(1));
  draw_field(&field, TRUE);
  refresh();

  while ((ch = getch()) != KEY_F(1)) {
    draw_field(&field, TRUE);
    refresh();
  }

  endwin();
  return 0;
}
