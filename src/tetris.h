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

typedef struct {
  int **field;
  int **next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

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

// frontend
void draw_field(GameField *field, WINDOW *win);
void draw_tetromino(Tetromino *t, WINDOW *win);
void draw_box(WINDOW *win);
void render_all(WINDOW *borders_win, WINDOW *game_win, GameField *field,
                Tetromino *t);
