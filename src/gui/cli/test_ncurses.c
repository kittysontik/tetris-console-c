#include <ncurses.h>
#include <stdlib.h>

// #include "tetris.h"
#define FIELD_HEIGHT 20
#define FIELD_WIDTH 10

typedef struct {
  int cells[FIELD_HEIGHT][FIELD_WIDTH];
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
  for (int y = 1; y <= FIELD_HEIGHT; y++) {
    for (int x = 1; x <= FIELD_WIDTH; x++) {
      field->cells[y][x] = 0;
    }
  }
}

void draw_corners(GameField *field) {
  mvaddch(0, 0, ACS_ULCORNER);
  mvaddch(0, FIELD_WIDTH + 1, ACS_URCORNER);
  mvaddch(FIELD_HEIGHT + 1, 0, ACS_LLCORNER);
  mvaddch(FIELD_HEIGHT + 1, FIELD_WIDTH + 1, ACS_LRCORNER);
}

// пропускаем corners
void draw_borders(GameField *field) {
  mvhline(0, 1, ACS_HLINE, FIELD_WIDTH);
  mvhline(FIELD_HEIGHT + 1, 1, ACS_HLINE, FIELD_WIDTH);

  mvvline(1, 0, ACS_VLINE, FIELD_HEIGHT);
  mvvline(1, FIELD_WIDTH + 1, ACS_VLINE, FIELD_HEIGHT);
}

void clear_field(GameField *field) {
  for (int i = 1; i < FIELD_HEIGHT + 1; i++) {
    mvhline(i, 1, ' ', FIELD_WIDTH);
  }
}

void draw_field_borders(GameField *field) {
  draw_corners(field);
  draw_borders(field);
}

void draw_field(GameField *field) {
  draw_field_borders(field);
  for (int y = 1; y <= FIELD_HEIGHT; y++) {
    for (int x = 1; x <= FIELD_WIDTH; x++) {
      if (field->cells[y][x] == 1) {
        mvprintw(y, x, "#");
      } else {
        mvprintw(y, x, ".");
      }
    }
  }
  refresh();
}

void move_down(Tetromino *t) {
  for (int i = 0; i < 4; i++) {
    t->blocks[i].y += 1;
  }
}
void move_right(Tetromino *t) {
  for (int i = 0; i < 4; i++) {
    t->blocks[i].x += 1;
  }
}
void move_left(Tetromino *t) {
  for (int i = 0; i < 4; i++) {
    t->blocks[i].x -= 1;
  }
}

bool is_collision_below(Tetromino *t, GameField *field) {
  for (int i = 0; i < 4; i++) {
    int next_y = t->blocks[i].y + 1;
    int x = t->blocks[i].x;

    if (next_y >= FIELD_HEIGHT || field->cells[next_y][x] == 1) {
      return true;
    }
  }
  return false;
}

void stick_to_bottom(Tetromino *t, GameField *field) {
  for (int i = 0; i < 4; i++) {
    field->cells[t->blocks[i].y][t->blocks[i].x] = 1;
  }
}

int generate_rand_tetromino() { return rand() % TETROMINO_COUNT; }

void game_loop(Tetromino *t, GameField *field) {
  if (!is_collision_below(t, field)) {
    move_down(t);
  } else {
    stick_to_bottom(t, field);
    init_tetromino(t, generate_rand_tetromino());
  }
}

void draw_tetromino(Tetromino *t) {
  for (int i = 0; i < 4; i++) {
    mvprintw(t->blocks[i].y, t->blocks[i].x, "#");
  }
}

int main(void) {
  GameField field;
  int ch;
  Tetromino t;

  initscr();
  start_color();
  cbreak();
  keypad(stdscr, TRUE);
  noecho();
  refresh();

  init_field(&field);
  init_tetromino(&t, generate_rand_tetromino());
  draw_tetromino(&t);
  mvprintw(1, 15, "Press F1 to exit. Use arrow keys to move the square");

  draw_field(&field);

  while ((ch = getch()) != KEY_F(1)) {
    mvprintw(1, 15, "Press F1 to exit. Use arrow keys to move the square");
    init_tetromino(&t, generate_rand_tetromino());
    draw_field(&field);
    draw_tetromino(&t);

    if (ch == KEY_DOWN) {
      move_down(&t);
    }
    if (is_collision_below(&t, &field)) {
      stick_to_bottom(&t, &field);
      init_tetromino(&t, generate_rand_tetromino());
    }

    refresh();
  }

  endwin();
  return 0;
}
