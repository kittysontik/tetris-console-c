#pragma once

#include <locale.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define FIELD_HEIGHT 20
#define FIELD_WIDTH 10
#define BLOCK_WIDTH 2
#define BORDER_PADDING 1
#define WIN_HEIGHT (FIELD_HEIGHT + BORDER_PADDING * 2)
#define WIN_WIDTH (FIELD_WIDTH * BLOCK_WIDTH + BORDER_PADDING * 2)
#define INFO_BOX_WIDTH 10
#define INFO_BOX_HEIGHT 5
#define OFFSET_Y ((getmaxy(stdscr) - WIN_HEIGHT) / 2)
#define OFFSET_X ((getmaxx(stdscr) - WIN_WIDTH) / 2)
#define RIGHT_PANEL_X (OFFSET_X + WIN_WIDTH + 1)
#define LEFT_PANEL_X (OFFSET_X - INFO_BOX_WIDTH - 1)

#define MENU_TITLE_Y 6
#define MENU_START_Y (MENU_TITLE_Y + 2)
#define MENU_PAUSE_Y (MENU_TITLE_Y + 3)
#define MENU_QUIT_Y (MENU_TITLE_Y + 4)
#define MENU_MOVE_Y (MENU_TITLE_Y + 5)
#define MENU_X_OFFSET 2
#define GAME_OVER_Y (FIELD_HEIGHT / 2)

#define MIN_SPEED 500000        // 0.5 секунды в микросекундах
#define LEVEL_SPEED_STEP 10000  // увеличение скорости на каждом уровене
#define MAX_SPEED 100000
#define SCORE_STEP 600
#define SHIFT_DELAY 3 * 100000  // задержка в 0.3 секунды в микросекундах

#define BLOCK_SYMBOL "\u2592"
#define EMPTY_SYMBOL "  "

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

typedef enum GameState { STATE_MENU, STATE_PLAYING, STATE_GAME_OVER, STATE_EXIT } GameState;

typedef struct GameInfo_t {
  GameField field;
  Tetromino current_tetromino;
  Tetromino next_tetromino;

  int score;
  int high_score;
  int level;
  long speed;
  int pause;

  GameState game_state;

  struct timespec last_fall;
} GameInfo_t;

typedef enum UserAction_t {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Down,
  Action,
  NoAction
} UserAction_t;

typedef struct GameWindows {
  WINDOW* menu_win;
  WINDOW* game_win;
  WINDOW* borders_win;
  WINDOW* next_win;
  WINDOW* score_win;
  WINDOW* highscore_win;
  WINDOW* level_win;

} GameWindows;

// frontend
GameWindows init_windows(void);
void init_ncurses(GameWindows* game_windows);
void delete_windows(GameWindows* game_windows);

void draw_field(GameInfo_t* game_info, GameWindows* game_windows);
void draw_tetromino(Tetromino* t, WINDOW* win);
void draw_next_tetromino(Tetromino* t, WINDOW* win);
void draw_box(WINDOW* win);
void render_all(GameWindows* game_windows, GameInfo_t* game_info);
void render_game_over(GameWindows* game_windows);
void draw_next_win(GameWindows* game_windows);
void render_menu(WINDOW* win);
void draw_score_win(GameWindows* game_windows, int score);
void draw_highscore_win(GameWindows* game_windows, int highscore);
void draw_level_win(GameWindows* game_windows, int level);

int center_text_x(WINDOW* win, const char* text);
void mvwprint_center(WINDOW* win, int y, const char* text);
void refresh_wins(GameWindows* game_windows);
void erase_wins(GameWindows* game_windows);

// backend
Tetromino init_tetromino(void);
GameField init_field(void);
GameInfo_t init_game_info(void);

void generate_next_tetromino(GameInfo_t* game_info);

void move_down(GameInfo_t* game_info);
void move_right(GameInfo_t* game_info, int direction);
void move_left(GameInfo_t* game_info, int direction);

bool is_out_of_borders(int x, int y);
bool is_cell_occupied(GameInfo_t* game_info, int x, int y);
bool is_game_over(GameInfo_t* game_info);
bool is_collision_on_sides(GameInfo_t* game_info, int direction);
bool is_collision_below(GameInfo_t* game_info);

bool can_rotate(GameInfo_t* game_info, Tetromino* rotated);
void rotate_tetromino(GameInfo_t* game_info);

void stick_to_bottom(GameInfo_t* game_info);

int generate_rand_tetromino(void);

long get_elapsed_time(struct timespec* last_time, struct timespec* current_time);

bool is_full_row(GameField* field, int row);
bool has_full_rows(GameField* field);
int clear_full_lines(GameInfo_t* game_info);
int load_high_score(void);
void save_high_score(int score);
void handle_full_lines(GameInfo_t* game_info);
int calculate_score(int cleared_lines);
void update_level(GameInfo_t* game_info);
void update_speed(GameInfo_t* game_info);
void update_score(GameInfo_t* game_info, int lines);

void handle_user_input(UserAction_t action, GameInfo_t* game_info, GameWindows* game_windows);

// states of finite state machine for tetris
void run_game_fsm(GameInfo_t* game_info, GameWindows* game_windows);

void handle_state_playing(GameInfo_t* game_info, GameWindows* game_windows);

void handle_input_if_any(GameInfo_t* game_info, GameWindows* game_windows);

void handle_tetromino_fall(GameInfo_t* game_info, GameWindows* game_windows,
                           struct timespec* current_time);

void check_game_over(GameInfo_t* game_info);

bool handle_stick(GameInfo_t* game_info, GameWindows* game_windows);

void handle_state_menu(GameInfo_t* game_info, GameWindows* game_windows);

void handle_state_game_over(GameInfo_t* game_info, GameWindows* game_windows);
UserAction_t map_key_to_action(int ch);

GameInfo_t update_current_state(GameInfo_t* game_info, GameWindows* game_windows);

void reset_game(GameInfo_t* game_info);
