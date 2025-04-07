#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// #include "tetris.h"
#define FIELD_HEIGHT 20
#define FIELD_WIDTH 10
#define WIN_HEIGHT 22
#define WIN_WIDTH 12

#define FALL_DELAY 0.5          // задержка в 0.5 секунды
#define SHIFT_DELAY 5 * 100000  // задержка в 0.5 секунды в микросекундах

typedef struct {
  int cells[FIELD_HEIGHT][FIELD_WIDTH];
} GameField;

typedef struct {
  int x, y;
} Block;

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

typedef struct {
  Block blocks[4];
  TetrominoType type;
} Tetromino;

void init_tetromino(Tetromino *t, TetrominoType type) {
  if (t == NULL) return;
  int center_x = FIELD_WIDTH / 2;
  t->type = type;

  const int shapes[7][4][2] = {
      {{-2, 0}, {-1, 0}, {0, 0}, {1, 0}},  // TETROMINO_I

      {{-1, 0}, {-1, 1}, {0, 1}, {1, 1}},  // TETROMINO_L

      {{1, 0}, {-1, 1}, {0, 1}, {1, 1}},  // TETROMINO_J

      {{-1, 0}, {0, 0}, {-1, 1}, {0, 1}},  // TETROMINO_O

      {{-1, 1}, {0, 1}, {0, 0}, {1, 0}},  // TETROMINO_S

      {{-1, 0}, {-2, 1}, {-1, 1}, {0, 1}},  // TETROMINO_T

      {{-1, 1}, {0, 1}, {-2, 0}, {-1, 0}},  // TETROMINO_Z
  };

  for (int i = 0; i < 4; i++) {
    t->blocks[i].x = center_x + shapes[type][i][0];
    t->blocks[i].y = shapes[type][i][1];
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

bool is_out_of_borders(int x, int y) {
  return (x < 0 || x >= FIELD_WIDTH || y >= FIELD_HEIGHT);
}

bool is_cell_occupied(GameField *field, int x, int y) {
  return (y >= 0 && field->cells[y][x] == 1);
}

bool is_game_over(Tetromino *t, GameField *field) {
  for (int i = 0; i < 4; i++) {
    int x = t->blocks[i].x;
    int y = t->blocks[i].y;
    if (is_cell_occupied(field, x, y)) {
      return true;
    }
  }
  return false;
}

bool is_collision_on_sides(Tetromino *t, GameField *field, int direction) {
  for (int i = 0; i < 4; i++) {
    int next_x = t->blocks[i].x + direction;
    int y = t->blocks[i].y;

    if (is_out_of_borders(next_x, y) || is_cell_occupied(field, next_x, y)) {
      return true;
    }
  }
  return false;
}

void move_right(Tetromino *t, GameField *field, int direction) {
  if (!is_collision_on_sides(t, field, direction)) {
    for (int i = 0; i < 4; i++) {
      t->blocks[i].x += 1;
    }
  }
}

void move_left(Tetromino *t, GameField *field, int direction) {
  if (!is_collision_on_sides(t, field, direction)) {
    for (int i = 0; i < 4; i++) {
      t->blocks[i].x -= 1;
    }
  }
}

bool is_collision_below(Tetromino *t, GameField *field) {
  for (int i = 0; i < 4; i++) {
    int next_y = t->blocks[i].y + 1;
    int x = t->blocks[i].x;

    if (is_out_of_borders(x, next_y) || is_cell_occupied(field, x, next_y)) {
      return true;
    }
  }
  return false;
}

bool can_rotate(Tetromino *rotated, GameField *field) {
  for (int i = 0; i < 4; i++) {
    int x = rotated->blocks[i].x;
    int y = rotated->blocks[i].y;

    if (is_out_of_borders(x, y) || is_cell_occupied(field, x, y)) {
      return false;
    }
  }
  return true;
}

void rotate_tetromino(Tetromino *t, GameField *field, TetrominoType type) {
  // if (!t || !field) {
  //   return;
  // }

  if (type == TETROMINO_O) {
    return;  // Квадрат не вращается
  }
  Tetromino rotated = *t;
  Block center = t->blocks[0];

  for (int i = 1; i < 4; i++) {
    int x = t->blocks[i].x - center.x;
    int y = t->blocks[i].y - center.y;

    rotated.blocks[i].x = center.x - y;
    rotated.blocks[i].y = center.y + x;
  }

  if (!can_rotate(&rotated, field)) {
    return;
  }

  *t = rotated;
}

void stick_to_bottom(Tetromino *t, GameField *field) {
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

void game_over(WINDOW *game_win) {
  wclear(game_win);
  mvwprintw(game_win, FIELD_HEIGHT / 2, FIELD_WIDTH / 2, "Game Over!");
  wrefresh(game_win);

  wgetch(game_win);
}

// 1 нс = 10 в -9 степени сек.
// перевод наносекунд в секунды
double get_elapsed_time(struct timespec *start, struct timespec *end) {
  return ((end->tv_sec - start->tv_sec) +
          (end->tv_nsec - start->tv_nsec) / 1e9);
}

bool is_full_row(GameField *field, int row) {
  for (int j = 0; j < FIELD_WIDTH; j++) {
    if (field->cells[row][j] == 0) {
      return false;
    }
  }
  return true;
}

bool has_full_rows(GameField *field) {
  for (int i = 0; i < FIELD_HEIGHT; i++) {
    if (is_full_row(field, i)) {
      return true;
    }
  }
  return false;
}

void shift_rows(GameField *field, WINDOW *game_win) {
  for (int row = 0; row < FIELD_HEIGHT; row++) {
    if (is_full_row(field, row)) {
      for (int k = row; k > 0; k--) {            // rows
        for (int j = 0; j < FIELD_WIDTH; j++) {  // columns
          field->cells[k][j] = field->cells[k - 1][j];
        }
      }
      for (int j = 0; j < FIELD_WIDTH; j++) {
        field->cells[0][j] = 0;  // обнуляем самую верхнюю строку
      }
      row--;
    }
  }
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

  clock_gettime(CLOCK_MONOTONIC, &last_fall);  // Фиксируем время

  while (running) {
    clock_gettime(CLOCK_MONOTONIC, &current_time);  // Получаем текущее время

    // Считаем, сколько времени прошло с последнего зафиксированного момента
    double elapsed_time = get_elapsed_time(&last_fall, &current_time);

    if (elapsed_time >= FALL_DELAY) {
      move_down(&t, game_win);
      if (is_collision_below(&t, &field)) {
        stick_to_bottom(&t, &field);
        if (has_full_rows(&field)) {
          draw_field(&field, game_win);
          // Подготавливаем окна для перерисовки
          wnoutrefresh(game_win);
          // Обновляем все окна
          doupdate();
          usleep(SHIFT_DELAY);
          shift_rows(&field, game_win);
        }
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
          stick_to_bottom(&t, &field);
          init_tetromino(&t, generate_rand_tetromino());
        }
        clock_gettime(CLOCK_MONOTONIC, &last_fall);
        break;

      case KEY_RIGHT:
        move_right(&t, &field, 1);
        break;

      case KEY_LEFT:
        move_left(&t, &field, -1);
        break;

      case KEY_UP:
        rotate_tetromino(&t, &field, t.type);
        break;
    }

    if (is_game_over(&t, &field)) {
      game_over(game_win);
      sleep(5);
      running = false;
    }

    werase(game_win);

    draw_box(borders_win);
    draw_field(&field, game_win);
    draw_tetromino(&t, game_win);

    // Подготавливаем окна для перерисовки
    wnoutrefresh(borders_win);
    wnoutrefresh(game_win);

    // Обновляем все окна
    doupdate();
  }

  endwin();
  return 0;
}
