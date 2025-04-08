#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define FIELD_HEIGHT 20
#define FIELD_WIDTH 10
#define WIN_HEIGHT 22
#define WIN_WIDTH 12
#define OFF_SET_Y ((getmaxy(stdscr) - WIN_HEIGHT) / 2)
#define OFF_SET_X ((getmaxx(stdscr) - WIN_WIDTH) / 2)

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

typedef struct {
  GameField field;
  Tetromino tetromino;
  Tetromino next_tetromino;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

// frontend
void init_ncurses(WINDOW *game_win, WINDOW *borders_win);
void init_colors();
void draw_field(GameField *field, WINDOW *win);
void draw_tetromino(Tetromino *t, WINDOW *win);
void draw_box(WINDOW *win);
void render_all(WINDOW *borders_win, WINDOW *game_win, WINDOW *next_win,
                GameField *field, Tetromino *t);
void render_game_over(WINDOW *game_win);
void draw_next_win(WINDOW *win);

// backend
Tetromino init_tetromino();
GameField init_field();
GameInfo_t init_game_info();

void move_down(GameInfo_t *game_info);
void move_right(GameInfo_t *game_info, int direction);
void move_left(GameInfo_t *game_info, int direction);

bool is_out_of_borders(int x, int y);
bool is_cell_occupied(GameInfo_t *game_info, int x, int y);
bool is_game_over(GameInfo_t *game_info);
bool is_collision_on_sides(GameInfo_t *game_info, int direction);
bool is_collision_below(GameInfo_t *game_info);

bool can_rotate(GameInfo_t *game_info);
void rotate_tetromino(GameInfo_t *game_info);

void stick_to_bottom(GameInfo_t *game_info);

int generate_rand_tetromino();

double get_elapsed_time(struct timespec *start, struct timespec *end);

bool is_full_row(GameField *field, int row);
bool has_full_rows(GameField *field);
void shift_rows(GameInfo_t *game_info);

// main loop of game
void game_loop(GameInfo_t *game_info, int ch, bool running,
               struct timespec last_fall, struct timespec current_time,
               WINDOW *borders_win, WINDOW *game_win, WINDOW *next_win);
