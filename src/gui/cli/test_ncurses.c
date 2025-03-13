#include <ncurses.h>

// #include "tetris.h"
#define FIELD_HEIGHT 22
#define FIELD_WIDTH 12

typedef struct {
  int x, y;
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

void init_tetromino(Tetromino *t, TetrominoType type) {
  if (t == NULL) return;

  switch (type) {
    case TETROMINO_I:
      t->blocks[0] = (Block){1, 1};
      t->blocks[1] = (Block){2, 1};
      t->blocks[2] = (Block){3, 1};
      t->blocks[3] = (Block){4, 1};
      break;

    case TETROMINO_L:
      t->blocks[0] = (Block){1, 1};
      t->blocks[1] = (Block){1, 2};
      t->blocks[2] = (Block){2, 2};
      t->blocks[3] = (Block){3, 2};
      break;

    case TETROMINO_J:
      t->blocks[0] = (Block){1, 2};
      t->blocks[1] = (Block){2, 2};
      t->blocks[2] = (Block){3, 2};
      t->blocks[3] = (Block){3, 1};
      break;

    case TETROMINO_O:
      t->blocks[0] = (Block){1, 1};
      t->blocks[1] = (Block){1, 2};
      t->blocks[2] = (Block){2, 2};
      t->blocks[3] = (Block){2, 1};
      break;

    case TETROMINO_S:
      t->blocks[0] = (Block){1, 2};
      t->blocks[1] = (Block){2, 2};
      t->blocks[2] = (Block){2, 1};
      t->blocks[3] = (Block){3, 1};
      break;

    case TETROMINO_T:
      t->blocks[0] = (Block){2, 1};
      t->blocks[1] = (Block){1, 2};
      t->blocks[2] = (Block){2, 2};
      t->blocks[3] = (Block){3, 2};
      break;

    case TETROMINO_Z:

      t->blocks[0] = (Block){1, 1};
      t->blocks[1] = (Block){2, 1};
      t->blocks[2] = (Block){2, 2};
      t->blocks[3] = (Block){3, 2};
      break;

    default:
      t->blocks[0] = (Block){1, 1};
      t->blocks[1] = (Block){2, 1};
      t->blocks[2] = (Block){3, 1};
      t->blocks[3] = (Block){4, 1};
      break;
  }
}

void init_field(GameField *field) {
  field->x = 0;
  field->y = 0;
}

void draw_corners(GameField *field) {
  mvaddch(field->y, field->x, ACS_ULCORNER);
  mvaddch(field->y, (field->x + FIELD_WIDTH - 1), ACS_URCORNER);
  mvaddch((field->y + FIELD_HEIGHT - 1), field->x, ACS_LLCORNER);
  mvaddch((field->y + FIELD_HEIGHT - 1), (field->x + FIELD_WIDTH - 1),
          ACS_LRCORNER);
}

// пропускаем corners
void draw_borders(GameField *field) {
  mvhline(field->y, field->x + 1, ACS_HLINE, FIELD_WIDTH - 2);
  mvhline(field->y + FIELD_HEIGHT - 1, field->x + 1, ACS_HLINE,
          FIELD_WIDTH - 2);

  mvvline(field->y + 1, field->x, ACS_VLINE, FIELD_HEIGHT - 2);
  mvvline(field->y + 1, field->x + FIELD_WIDTH - 1, ACS_VLINE,
          FIELD_HEIGHT - 2);
}

void fill_field(GameField *field) {
  for (int i = field->y + 1; i < field->y + FIELD_HEIGHT - 1; i++) {
    mvhline(i, field->x + 1, '.', FIELD_WIDTH - 2);
  }
}

void clear_field(GameField *field) {
  for (int j = field->y; j < field->y + FIELD_HEIGHT; j++) {
    for (int i = field->x; i < field->x + FIELD_WIDTH; i++) {
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

void draw_tetromino(Tetromino *t) {
  for (int i = 0; i < 4; i++) {
    mvprintw(t->blocks[i].y, t->blocks[i].x, "#");
  }
}

int main(void) {
  GameField field;
  int ch;
  Tetromino figure;

  initscr();
  start_color();
  cbreak();
  keypad(stdscr, TRUE);
  noecho();
  refresh();

  init_pair(1, COLOR_CYAN, COLOR_BLACK);

  init_field(&field);
  init_tetromino(&figure, 6);

  attron(COLOR_PAIR(1));
  mvprintw(1, 15, "Press F1 to exit. Use arrow keys to move the square");
  refresh();

  attroff(COLOR_PAIR(1));
  draw_field(&field, TRUE);
  draw_tetromino(&figure);
  refresh();

  while ((ch = getch()) != KEY_F(1)) {
    draw_field(&field, TRUE);
    draw_tetromino(&figure);

    refresh();
  }

  endwin();
  return 0;
}
