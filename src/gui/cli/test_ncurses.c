#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// #include "tetris.h"
#define FIELD_HEIGHT 22
#define FIELD_WIDTH 12

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
  for (int y = 0; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      field->cells[y][x] = 0;
    }
  }
}

void draw_corners() {
  mvaddch(0, 0, ACS_ULCORNER);
  mvaddch(0, FIELD_WIDTH - 1, ACS_URCORNER);
  mvaddch(FIELD_HEIGHT - 1, 0, ACS_LLCORNER);
  mvaddch(FIELD_HEIGHT - 1, FIELD_WIDTH - 1, ACS_LRCORNER);
}

// пропускаем corners
void draw_borders() {
  mvhline(0, 1, ACS_HLINE, FIELD_WIDTH - 2);
  mvhline(FIELD_HEIGHT - 1, 1, ACS_HLINE, FIELD_WIDTH - 2);

  mvvline(1, 0, ACS_VLINE, FIELD_HEIGHT - 2);
  mvvline(1, FIELD_WIDTH - 1, ACS_VLINE, FIELD_HEIGHT - 2);
}

void draw_field_borders() {
  draw_corners();
  draw_borders();
}

void draw_field(GameField *field) {
  for (int y = 1; y < FIELD_HEIGHT - 1; y++) {
    for (int x = 1; x < FIELD_WIDTH - 1; x++) {
      if (field->cells[y][x] == 1) {
        mvprintw(y, x, "#");
      } else {
        mvprintw(y, x, ".");
      }
    }
  }
}

void move_down(Tetromino *t) {
  for (int i = 0; i < 4; i++) {
    if (t->blocks[i].y <= FIELD_HEIGHT) t->blocks[i].y += 1;
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

    if (next_y == FIELD_HEIGHT || field->cells[next_y][x] == 1) {
      return true;
    }
  }
  return false;
}

void stick_to_bottom(Tetromino *t, GameField *field) {
  for (int i = 0; i < 4; i++) {
    if (t->blocks[i].y <= FIELD_HEIGHT && t->blocks[i].x <= FIELD_WIDTH)
      field->cells[t->blocks[i].y][t->blocks[i].x] = 1;
  }
}

int generate_rand_tetromino() {
  int result;
  srand(clock());
  for (int i = 0; i < 10; i++) {
    result = rand() % TETROMINO_COUNT;
  }
  return result;
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
  bool running = true;

  initscr();
  start_color();
  cbreak();
  keypad(stdscr, TRUE);
  noecho();
  curs_set(0);

  init_field(&field);
  init_tetromino(&t, generate_rand_tetromino());
  draw_field_borders();

  draw_field(&field);

  draw_tetromino(&t);
  mvprintw(1, 15, "Press F1 to exit. Use arrow keys to move the square");

  refresh();
  ch = getch();

  // while (running) {
  //   ch = getch();

  //   switch (ch) {
  //     case 'q':
  //       running = false;
  //       break;

  //     case KEY_DOWN:
  //       move_down(&t);
  //       if (is_collision_below(&t, &field)) {
  //         stick_to_bottom(&t, &field);
  //         init_tetromino(&t, generate_rand_tetromino());
  //       }
  //       mvprintw(1, 15, "Press F1 to exit. Use arrow keys to move the
  //       square"); draw_field(&field); draw_tetromino(&t); break;

  //     case KEY_RIGHT:
  //       move_right(&t);
  //       mvprintw(1, 15, "Press F1 to exit. Use arrow keys to move the
  //       square"); draw_field(&field); draw_tetromino(&t); break;

  //     case KEY_LEFT:
  //       move_left(&t);
  //       mvprintw(1, 15, "Press F1 to exit. Use arrow keys to move the
  //       square"); draw_field(&field); draw_tetromino(&t); break;
  //   }

  //   refresh();
  // }
  endwin();
  return 0;
}
