#include "../../tetris.h"

Tetromino init_tetromino() {
  Tetromino t;
  int center_x = FIELD_WIDTH / 2;
  t.type = generate_rand_tetromino();

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
    t.blocks[i].x = center_x + shapes[t.type][i][0];
    t.blocks[i].y = shapes[t.type][i][1];
  }
  return t;
}

GameField init_field() {
  GameField field;
  for (int y = 0; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      field.cells[y][x] = 0;
    }
  }
  return field;
}

GameInfo_t init_game_info() {
  GameInfo_t game_info;

  game_info.score = 0;
  game_info.high_score = 0;
  game_info.pause = 0;
  game_info.speed = 0;
  game_info.level = 0;

  game_info.field = init_field();
  game_info.tetromino = init_tetromino();
  game_info.next_tetromino = init_tetromino();

  return game_info;
}

void move_down(Tetromino *t) {
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

bool can_rotate(Tetromino *t, GameField *field) {
  for (int i = 0; i < 4; i++) {
    int x = t->blocks[i].x;
    int y = t->blocks[i].y;

    if (is_out_of_borders(x, y) || is_cell_occupied(field, x, y)) {
      return false;
    }
  }
  return true;
}

void rotate_tetromino(Tetromino *t, GameField *field, TetrominoType type) {
  if (!t || !field) {
    return;
  }

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
  for (int i = 0; i < 20; i++) {
    result = rand() % TETROMINO_COUNT;
  }
  return result;
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

void shift_rows(GameField *field) {
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

void game_loop(GameField field, int ch, Tetromino t, bool running,
               struct timespec last_fall, struct timespec current_time,
               WINDOW *borders_win, WINDOW *game_win, WINDOW *next_win) {
  while (running) {
    clock_gettime(CLOCK_MONOTONIC, &current_time);  // Получаем текущее время

    // Считаем, сколько времени прошло с последнего зафиксированного момента
    double elapsed_time = get_elapsed_time(&last_fall, &current_time);

    if (elapsed_time >= FALL_DELAY) {
      move_down(&t);
      if (is_collision_below(&t, &field)) {
        stick_to_bottom(&t, &field);
        if (has_full_rows(&field)) {
          render_all(borders_win, game_win, next_win, &field, &t);

          usleep(SHIFT_DELAY);
          shift_rows(&field);
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
        move_down(&t);
        if (is_collision_below(&t, &field)) {
          stick_to_bottom(&t, &field);
          if (has_full_rows(&field)) {
            render_all(borders_win, game_win, next_win, &field, &t);

            usleep(SHIFT_DELAY);
            shift_rows(&field);
          }
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
      render_game_over(game_win);
      sleep(5);
      running = false;
    }

    render_all(borders_win, game_win, next_win, &field, &t);
  }
}
