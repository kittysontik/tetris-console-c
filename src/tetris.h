#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define FIELD_HEIGHT 20
#define FIELD_WIDTH 10
#define WIN_HEIGHT 22
#define WIN_WIDTH 12
#define OFFSET_Y ((getmaxy(stdscr) - WIN_HEIGHT) / 2)
#define OFFSET_X ((getmaxx(stdscr) - WIN_WIDTH) / 2)

#define FALL_DELAY 0.5          // задержка в 0.5 секунды
#define SHIFT_DELAY 3 * 100000  // задержка в 0.5 секунды в микросекундах

typedef struct GameField {
  int cells[FIELD_HEIGHT][FIELD_WIDTH];
} GameField;

typedef struct Block {
  int x, y;
} Block;

typedef enum TetrominoType {
  TETROMINO_I,  // линия
  TETROMINO_L,  // Г-образная
  TETROMINO_J,  // обратная Г-образная
  TETROMINO_O,  // квадрат
  TETROMINO_S,  // s-образная
  TETROMINO_T,  // т-образная
  TETROMINO_Z,  // z-образная
  TETROMINO_COUNT
} TetrominoType;

typedef struct Tetromino {
  Block blocks[4];
  TetrominoType type;
} Tetromino;

typedef enum GameState {
  STATE_MENU,
  STATE_PLAYING,
  STATE_PAUSED,
  STATE_GAME_OVER,
  STATE_EXIT
} GameState;

typedef struct GameInfo_t {
  GameField field;
  Tetromino current_tetromino;
  Tetromino next_tetromino;

  int score;
  int high_score;
  int level;
  int speed;
  int pause;

  GameState game_state;
} GameInfo_t;

typedef struct GameWindows {
  WINDOW *menu_win;
  WINDOW *game_win;
  WINDOW *borders_win;
  WINDOW *next_win;

} GameWindows;

// frontend
GameWindows init_windows();
void init_ncurses(GameWindows *game_windows);

void draw_field(GameInfo_t *game_info, GameWindows *game_windows);
void draw_tetromino(Tetromino *t, WINDOW *win);
void draw_next_tetromino(Tetromino *t, WINDOW *win);
void draw_box(WINDOW *win);
void render_all(GameWindows *game_windows, GameInfo_t *game_info);
void render_game_over(GameWindows *game_windows);
void draw_next_win(GameWindows *game_windows);
void render_menu(WINDOW *win);

// backend
Tetromino init_tetromino();
GameField init_field();
GameInfo_t init_game_info();

void generate_next_tetromino(GameInfo_t *game_info);

void move_down(GameInfo_t *game_info);
void move_right(GameInfo_t *game_info, int direction);
void move_left(GameInfo_t *game_info, int direction);

bool is_out_of_borders(int x, int y);
bool is_cell_occupied(GameInfo_t *game_info, int x, int y);
bool is_game_over(GameInfo_t *game_info);
bool is_collision_on_sides(GameInfo_t *game_info, int direction);
bool is_collision_below(GameInfo_t *game_info);

bool can_rotate(GameInfo_t *game_info, Tetromino *rotated);
void rotate_tetromino(GameInfo_t *game_info);

void stick_to_bottom(GameInfo_t *game_info);

int generate_rand_tetromino();

double get_elapsed_time(struct timespec *start, struct timespec *end);

bool is_full_row(GameField *field, int row);
bool has_full_rows(GameField *field);
void shift_rows(GameInfo_t *game_info);

void handle_user_input(int ch, GameInfo_t *game_info,
                       struct timespec *last_fall, GameWindows *game_windows);

// playing cycle for STATE_PLAYING
void game_loop(GameInfo_t *game_info, GameWindows *game_windows);

// states of finite state machine for tetris
void run_game_state_machine(GameInfo_t *game_info, GameWindows *game_windows);

void handle_state_playing(GameInfo_t *game_info, GameWindows *game_windows);

void handle_input_if_any(GameInfo_t *game_info, GameWindows *game_windows,
                         struct timespec *last_fall);

void handle_tetromino_fall(GameInfo_t *game_info, GameWindows *game_windows,
                           struct timespec *last_fall,
                           struct timespec *current_time);

void check_game_over(GameInfo_t *game_info);

bool handle_stick(GameInfo_t *game_info, GameWindows *game_windows);