#include "../../tetris.h"

int get_min_y_from_tetromino(const int shape[4][2]) {
  int min_y = shape[0][1];  // инициализиурем минимальную y-координату первым
                            // блоком тетромино координатой y
  for (int i = 1; i < 4; i++) {
    if (shape[i][1] < min_y) {
      min_y = shape[i][1];  // находим минимиальную y-координату в тетромино
    }
  }
  return min_y;
}

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

  const int (*shape)[2] = shapes[t.type];  // двумерный массив
  int min_y = get_min_y_from_tetromino(shape);

  for (int i = 0; i < 4; i++) {
    t.blocks[i].x = center_x + shapes[t.type][i][0];
    t.blocks[i].y = shapes[t.type][i][1] - min_y;
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

int load_high_score() {
  FILE *file = fopen("highscore.txt", "r");
  int score = 0;
  if (file) {
    fscanf(file, "%d", &score);
    fclose(file);
  }
  return score;
}

GameInfo_t init_game_info() {
  GameInfo_t game_info;

  game_info.score = 0;
  game_info.high_score = load_high_score();
  game_info.pause = 0;
  game_info.speed = 0;
  game_info.level = 0;

  game_info.field = init_field();
  game_info.current_tetromino = init_tetromino();
  game_info.next_tetromino = init_tetromino();

  game_info.game_state = STATE_MENU;

  // обнуляем таймер
  game_info.last_fall.tv_sec = 0;
  game_info.last_fall.tv_nsec = 0;

  return game_info;
}

void move_down(GameInfo_t *game_info) {
  if (is_collision_below(game_info)) {
    return;
  }
  for (int i = 0; i < 4; i++) {
    game_info->current_tetromino.blocks[i].y += 1;
  }
}

bool is_out_of_borders(int x, int y) {
  return (x < 0 || x >= FIELD_WIDTH || y >= FIELD_HEIGHT);
}

bool is_cell_occupied(GameInfo_t *game_info, int x, int y) {
  return (y >= 0 && game_info->field.cells[y][x] == 1);
}

bool is_game_over(GameInfo_t *game_info) {
  for (int i = 0; i < 4; i++) {
    int x = game_info->current_tetromino.blocks[i].x;
    int y = game_info->current_tetromino.blocks[i].y;
    if (is_cell_occupied(game_info, x, y)) {
      return true;
    }
  }
  return false;
}

bool is_collision_on_sides(GameInfo_t *game_info, int direction) {
  for (int i = 0; i < 4; i++) {
    int next_x = game_info->current_tetromino.blocks[i].x + direction;
    int y = game_info->current_tetromino.blocks[i].y;

    if (is_out_of_borders(next_x, y) ||
        is_cell_occupied(game_info, next_x, y)) {
      return true;
    }
  }
  return false;
}

void move_right(GameInfo_t *game_info, int direction) {
  if (!is_collision_on_sides(game_info, direction)) {
    for (int i = 0; i < 4; i++) {
      game_info->current_tetromino.blocks[i].x += 1;
    }
  }
}

void move_left(GameInfo_t *game_info, int direction) {
  if (!is_collision_on_sides(game_info, direction)) {
    for (int i = 0; i < 4; i++) {
      game_info->current_tetromino.blocks[i].x -= 1;
    }
  }
}

bool is_collision_below(GameInfo_t *game_info) {
  for (int i = 0; i < 4; i++) {
    int next_y = game_info->current_tetromino.blocks[i].y + 1;
    int x = game_info->current_tetromino.blocks[i].x;

    if (is_out_of_borders(x, next_y) ||
        is_cell_occupied(game_info, x, next_y)) {
      return true;
    }
  }
  return false;
}

bool can_rotate(GameInfo_t *game_info, Tetromino *rotated) {
  for (int i = 0; i < 4; i++) {
    int x = rotated->blocks[i].x;
    int y = rotated->blocks[i].y;

    if (is_out_of_borders(x, y) || is_cell_occupied(game_info, x, y)) {
      return false;
    }
  }
  return true;
}

void rotate_tetromino(GameInfo_t *game_info) {
  if (game_info->current_tetromino.type == TETROMINO_O) {
    return;  // Квадрат не вращается
  }
  Tetromino rotated = game_info->current_tetromino;
  Block center = rotated.blocks[0];

  for (int i = 1; i < 4; i++) {
    int x = game_info->current_tetromino.blocks[i].x - center.x;
    int y = game_info->current_tetromino.blocks[i].y - center.y;

    rotated.blocks[i].x = center.x - y;
    rotated.blocks[i].y = center.y + x;
  }

  if (!can_rotate(game_info, &rotated)) {
    return;
  }

  game_info->current_tetromino = rotated;
}

void stick_to_bottom(GameInfo_t *game_info) {
  for (int i = 0; i < 4; i++) {
    int y = game_info->current_tetromino.blocks[i].y;
    int x = game_info->current_tetromino.blocks[i].x;

    if (y >= 0 && y <= FIELD_HEIGHT && x >= 0 && x <= FIELD_WIDTH) {
      game_info->field.cells[y][x] = 1;
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

int clear_full_lines(GameInfo_t *game_info) {
  int cleared_lines = 0;
  // перед удалением полной строки делаем паузу
  usleep(SHIFT_DELAY);

  for (int row = 0; row < FIELD_HEIGHT; row++) {
    if (is_full_row(&game_info->field, row)) {
      cleared_lines++;
      for (int k = row; k > 0; k--) {            // rows
        for (int j = 0; j < FIELD_WIDTH; j++) {  // columns
          game_info->field.cells[k][j] = game_info->field.cells[k - 1][j];
        }
      }
      for (int j = 0; j < FIELD_WIDTH; j++) {
        game_info->field.cells[0][j] = 0;  // обнуляем самую верхнюю строку
      }
      row--;
    }
  }
  return cleared_lines;
}

void save_high_score(int score) {
  FILE *file = fopen("highscore.txt", "w");
  if (file) {
    fprintf(file, "%d", score);
    fclose(file);
  }
}

int calculate_score(int cleared_lines) {
  switch (cleared_lines) {
    case 1:
      return 100;
    case 2:
      return 300;
    case 3:
      return 700;
    case 4:
      return 1500;
    default:
      return 0;
  }
}

void handle_full_lines(GameInfo_t *game_info) {
  int lines = clear_full_lines(game_info);
  int gained = calculate_score(lines);
  game_info->score += gained;

  if (game_info->score > game_info->high_score) {
    game_info->high_score = game_info->score;
  }
}

void generate_next_tetromino(GameInfo_t *game_info) {
  game_info->current_tetromino = game_info->next_tetromino;
  game_info->next_tetromino = init_tetromino();
}

void handle_user_input(UserAction_t action, GameInfo_t *game_info,
                       GameWindows *game_windows) {
  switch (action) {
    case Terminate:
      game_info->game_state = STATE_EXIT;
      break;

    case Pause:
      game_info->pause = !game_info->pause;
      break;

    case Start:
      game_info->pause = !game_info->pause;  // дублирует 'p'
      break;

    case Down:
      if (game_info->pause) {
        return;
      }
      move_down(game_info);
      handle_stick(game_info, game_windows);
      break;

    case Right:
      if (!game_info->pause) {
        move_right(game_info, 1);
      }
      break;

    case Left:
      if (!game_info->pause) {
        move_left(game_info, -1);
      }
      break;

    case Action:
      if (!game_info->pause) {
        rotate_tetromino(game_info);
      }
      break;

    default:
      // Ничего не делаем для неизвестных действий
      break;
  }
}

void run_game_fsm(GameInfo_t *game_info, GameWindows *game_windows) {
  while (game_info->game_state != STATE_EXIT) {
    switch (game_info->game_state) {
      case STATE_MENU:
        handle_state_menu(game_info, game_windows);
        break;

      case STATE_PLAYING:
        handle_state_playing(game_info, game_windows);
        break;

      case STATE_GAME_OVER:
        handle_state_game_over(game_info, game_windows);
        break;

      default:
        game_info->game_state = STATE_EXIT;
        break;
    }
  }
}

void handle_state_playing(GameInfo_t *game_info, GameWindows *game_windows) {
  *game_info = update_current_state(game_info, game_windows);
  render_all(game_windows, game_info);
}

void handle_input_if_any(GameInfo_t *game_info, GameWindows *game_windows) {
  int ch = wgetch(game_windows->game_win);
  if (ch == ERR) return;

  UserAction_t action = map_key_to_action(ch);
  if (action == -1) return;

  handle_user_input(action, game_info, game_windows);
}

void handle_tetromino_fall(GameInfo_t *game_info, GameWindows *game_windows,
                           struct timespec *current_time) {
  double elapsed_time = get_elapsed_time(&game_info->last_fall, current_time);

  if (!game_info->pause && elapsed_time >= FALL_DELAY) {
    bool is_stick_success = handle_stick(game_info, game_windows);
    if (!is_stick_success) {
      move_down(game_info);
    }
    // сбрасываем таймер
    clock_gettime(CLOCK_MONOTONIC, &game_info->last_fall);
  }
}

bool handle_stick(GameInfo_t *game_info, GameWindows *game_windows) {
  if (!is_collision_below(game_info)) {
    return false;
  }
  stick_to_bottom(game_info);
  if (has_full_rows(&game_info->field)) {
    render_all(game_windows, game_info);
    handle_full_lines(game_info);
  }
  generate_next_tetromino(game_info);
  return true;
}

void check_game_over(GameInfo_t *game_info) {
  if (is_game_over(game_info)) {
    game_info->game_state = STATE_GAME_OVER;
    save_high_score(game_info->high_score);
  }
}

void handle_state_menu(GameInfo_t *game_info, GameWindows *game_windows) {
  render_menu(game_windows->menu_win);

  int ch = wgetch(game_windows->menu_win);
  UserAction_t action = map_key_to_action(ch);
  switch (action) {
    case Start:
      // Фиксируем время перед началом игры
      clock_gettime(CLOCK_MONOTONIC, &game_info->last_fall);
      game_info->game_state = STATE_PLAYING;
      werase(game_windows->menu_win);
      wrefresh(game_windows->menu_win);
      break;

    case Terminate:
      game_info->game_state = STATE_EXIT;
      break;

    default:
      // Остальные действия не нужны в меню
      break;
  }
}

void handle_state_game_over(GameInfo_t *game_info, GameWindows *game_windows) {
  render_game_over(game_windows);
  sleep(3);
  game_info->game_state = STATE_EXIT;
}

UserAction_t map_key_to_action(int ch) {
  switch (ch) {
    case 'q':
      return Terminate;
    case 'p':
      return Pause;
    case 's':
      return Start;
    case KEY_LEFT:
      return Left;
    case KEY_RIGHT:
      return Right;
    case KEY_DOWN:
      return Down;
    case KEY_UP:
      return Action;  // Вращение фигуры
    default:
      return -1;  // Неизвестная клавиша
  }
}

GameInfo_t update_current_state(GameInfo_t *game_info,
                                GameWindows *game_windows) {
  if (game_info->game_state != STATE_PLAYING) {
    return *game_info;
  }
  struct timespec current_time;
  // Получаем текущее время
  clock_gettime(CLOCK_MONOTONIC, &current_time);

  handle_tetromino_fall(game_info, game_windows, &current_time);

  handle_input_if_any(game_info, game_windows);

  check_game_over(game_info);

  return *game_info;
}
