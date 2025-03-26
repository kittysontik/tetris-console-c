#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// #include "tetris.h"
#define FIELD_HEIGHT 20
#define FIELD_WIDTH 10
#define WIN_HEIGHT 22
#define WIN_WIDTH 12
#define FALL_DELAY 500000

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
      t->blocks[0] = (Block){0, 0};
      t->blocks[1] = (Block){1, 0};
      t->blocks[2] = (Block){2, 0};
      t->blocks[3] = (Block){3, 0};
      break;

    case TETROMINO_L:
      t->blocks[0] = (Block){0, 0};
      t->blocks[1] = (Block){0, 1};
      t->blocks[2] = (Block){1, 1};
      t->blocks[3] = (Block){2, 1};
      break;

    case TETROMINO_J:
      t->blocks[0] = (Block){0, 1};
      t->blocks[1] = (Block){1, 1};
      t->blocks[2] = (Block){2, 1};
      t->blocks[3] = (Block){2, 0};
      break;

    case TETROMINO_O:
      t->blocks[0] = (Block){0, 0};
      t->blocks[1] = (Block){0, 1};
      t->blocks[2] = (Block){1, 1};
      t->blocks[3] = (Block){1, 0};
      break;

    case TETROMINO_S:
      t->blocks[0] = (Block){0, 1};
      t->blocks[1] = (Block){1, 1};
      t->blocks[2] = (Block){1, 0};
      t->blocks[3] = (Block){2, 0};
      break;

    case TETROMINO_T:
      t->blocks[0] = (Block){1, 0};
      t->blocks[1] = (Block){0, 1};
      t->blocks[2] = (Block){1, 1};
      t->blocks[3] = (Block){2, 1};
      break;

    case TETROMINO_Z:
      t->blocks[0] = (Block){0, 0};
      t->blocks[1] = (Block){1, 0};
      t->blocks[2] = (Block){1, 1};
      t->blocks[3] = (Block){2, 1};
      break;

    default:
      t->blocks[0] = (Block){0, 0};
      t->blocks[1] = (Block){1, 0};
      t->blocks[2] = (Block){2, 0};
      t->blocks[3] = (Block){3, 0};
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

void move_down(Tetromino *t, WINDOW *game_win) {
  for (int i = 0; i < 4; i++) {
    if (t->blocks[i].y < FIELD_HEIGHT) {
      t->blocks[i].y += 1;
    }
  }
}

bool is_right_side(Tetromino *t) {
  for (int i = 0; i < 4; i++) {
    if (t->blocks[i].x >= FIELD_WIDTH - 1) {
      return true;
    }
  }
  return false;
}

bool is_left_side(Tetromino *t) {
  for (int i = 0; i < 4; i++) {
    if (t->blocks[i].x <= 0) {
      return true;
    }
  }
  return false;
}

void move_right(Tetromino *t) {
  if (!is_right_side(t)) {
    for (int i = 0; i < 4; i++) {
      t->blocks[i].x += 1;
    }
  }
}

void move_left(Tetromino *t) {
  if (!is_left_side(t)) {
    for (int i = 0; i < 4; i++) {
      t->blocks[i].x -= 1;
    }
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

void stick_to_bottom(Tetromino *t, GameField *field, WINDOW *game_win) {
  for (int i = 0; i < 4; i++) {
    int y = t->blocks[i].y;
    int x = t->blocks[i].x;

    if (y >= 0 && y <= FIELD_HEIGHT && x >= 0 && x <= FIELD_WIDTH) {
      field->cells[y][x] = 1;
    }
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

int main(void) {
  initscr();

  int off_set_x = 0, off_set_y = 0;
  off_set_y = ((getmaxy(stdscr) - WIN_HEIGHT) / 2);
  off_set_x = (getmaxx(stdscr) - WIN_WIDTH) / 2;
  WINDOW *borders_win = newwin(WIN_HEIGHT, WIN_WIDTH, off_set_y, off_set_x);
  WINDOW *game_win =
      newwin(FIELD_HEIGHT, FIELD_WIDTH, off_set_y + 1, off_set_x + 1);

  start_color();
  cbreak();
  keypad(stdscr, TRUE);
  keypad(game_win, TRUE);
  keypad(borders_win, TRUE);
  nodelay(game_win, TRUE);  // Не блокируем getch()
  noecho();
  curs_set(0);

  init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);

  GameField field;
  int ch;
  Tetromino t;
  bool running = true;

  init_field(&field);
  init_tetromino(&t, generate_rand_tetromino());

  draw_box(borders_win);
  draw_field(&field, game_win);
  draw_tetromino(&t, game_win);

  wrefresh(borders_win);
  wrefresh(game_win);

  struct timespec last_fall, current_time;
  clock_gettime(CLOCK_MONOTONIC, &last_fall);

  while (running) {
    clock_gettime(CLOCK_MONOTONIC, &current_time);  // Получаем текущее время

    long elapsed_time = (current_time.tv_sec - last_fall.tv_sec) * 1000000L +
                        (current_time.tv_nsec - last_fall.tv_nsec) / 1000L;

    if (elapsed_time >= FALL_DELAY) {
      move_down(&t, game_win);
      if (is_collision_below(&t, &field)) {
        stick_to_bottom(&t, &field, game_win);
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
          stick_to_bottom(&t, &field, game_win);
          init_tetromino(&t, generate_rand_tetromino());
        }
        clock_gettime(CLOCK_MONOTONIC, &last_fall);
        break;

      case KEY_RIGHT:
        move_right(&t);
        break;

      case KEY_LEFT:
        move_left(&t);
        break;
    }
    werase(game_win);

    draw_box(borders_win);
    draw_field(&field, game_win);
    draw_tetromino(&t, game_win);

    wnoutrefresh(borders_win);
    wnoutrefresh(game_win);

    doupdate();

    usleep(10000);
  }

  endwin();
  return 0;
}
