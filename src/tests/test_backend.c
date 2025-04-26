#include <check.h>

#include "../tetris.h"

START_TEST(test_handle_tetromino_fall)
{
  GameInfo_t game_info = init_game_info();
  game_info.pause = false;
  game_info.speed = 1;

  int initial_y = game_info.current_tetromino.blocks[0].y;

  clock_gettime(CLOCK_MONOTONIC, &game_info.last_fall);
  struct timespec current_time = game_info.last_fall;
  current_time.tv_nsec += 2000000;  // +2 мс

  handle_tetromino_fall(&game_info, NULL, &current_time);

  ck_assert_int_eq(game_info.current_tetromino.blocks[0].y, initial_y + 1);
}
END_TEST

START_TEST(test_handle_stick)
{
  GameInfo_t game_info = init_game_info();

  for (int i = 0; i < 4; i++) {
    game_info.current_tetromino.blocks[i].y = FIELD_HEIGHT - 1;
  }

  bool stuck = handle_stick(&game_info, NULL);
  ck_assert(stuck);

  for (int i = 0; i < 4; i++) {
    ck_assert_int_lt(game_info.current_tetromino.blocks[i].y, FIELD_HEIGHT / 2);
  }
}
END_TEST

START_TEST(test_move_down)
{
  GameInfo_t game_info = init_game_info();

  int initial_y = game_info.current_tetromino.blocks[0].y;

  move_down(&game_info);

  ck_assert_int_gt(game_info.current_tetromino.blocks[0].y, initial_y);
}
END_TEST

START_TEST(test_move_down_with_collision)
{
  GameInfo_t game_info = init_game_info();

  game_info.current_tetromino.blocks[0].y = FIELD_HEIGHT - 1;

  move_down(&game_info);

  ck_assert_int_eq(game_info.current_tetromino.blocks[0].y, FIELD_HEIGHT - 1);
}
END_TEST

START_TEST(test_generate_next_tetromino)
{
  GameInfo_t game_info = init_game_info();

  Tetromino current_tetromino = game_info.current_tetromino;
  generate_next_tetromino(&game_info);

  ck_assert_ptr_ne(&game_info.current_tetromino, &current_tetromino);
}
END_TEST

START_TEST(test_is_collision_below)
{
  GameInfo_t game_info = init_game_info();

  game_info.current_tetromino.blocks[0].y = FIELD_HEIGHT - 1;

  bool result = is_collision_below(&game_info);
  ck_assert(result);
}
END_TEST

START_TEST(test_stick_to_bottom)
{
  GameInfo_t game_info = init_game_info();

  for (int i = 0; i < 4; i++) {
    game_info.current_tetromino.blocks[i].y = FIELD_HEIGHT - 2;
  }

  stick_to_bottom(&game_info);

  for (int i = 0; i < 4; i++) {
    int y = game_info.current_tetromino.blocks[i].y;
    int x = game_info.current_tetromino.blocks[i].x;

    ck_assert_int_ge(y, 0);
    ck_assert_int_lt(y, FIELD_HEIGHT);
    ck_assert_int_ge(x, 0);
    ck_assert_int_lt(x, FIELD_WIDTH);

    ck_assert_int_eq(game_info.field.cells[y][x], 1);
  }
}
END_TEST

START_TEST(test_clear_full_lines)
{
  GameInfo_t game_info = init_game_info();

  for (int i = 0; i < FIELD_WIDTH; i++) {
    game_info.field.cells[FIELD_HEIGHT - 1][i] = 1;
  }

  clear_full_lines(&game_info);

  for (int i = 0; i < FIELD_WIDTH; i++) {
    ck_assert_int_eq(game_info.field.cells[FIELD_HEIGHT - 1][i], 0);
  }
}
END_TEST

START_TEST(test_generate_rand_tetromino)
{
  GameInfo_t game_info = init_game_info();

  int current_type = game_info.current_tetromino.type;

  game_info.current_tetromino = init_tetromino();

  ck_assert_int_ne(game_info.current_tetromino.type, current_type);
}
END_TEST

START_TEST(test_check_game_over)
{
  GameInfo_t game_info = init_game_info();

  for (int i = 0; i < FIELD_WIDTH; i++) {
    game_info.field.cells[0][i] = 1;
    game_info.field.cells[1][i] = 1;
  }

  bool game_over = is_game_over(&game_info);
  ck_assert(game_over);
}
END_TEST

START_TEST(test_reset_game)
{
  GameInfo_t game_info = init_game_info();
  reset_game(&game_info);

  for (int i = 0; i < 4; i++) {
    ck_assert_int_ge(game_info.current_tetromino.blocks[i].x, 0);
    ck_assert_int_lt(game_info.current_tetromino.blocks[i].x, FIELD_WIDTH);
    ck_assert_int_ge(game_info.current_tetromino.blocks[i].y, 0);
    ck_assert_int_lt(game_info.current_tetromino.blocks[i].y, FIELD_HEIGHT);
  }
}
END_TEST

START_TEST(test_rotate_tetromino)
{
  GameInfo_t game_info = init_game_info();

  Tetromino original = game_info.current_tetromino;

  rotate_tetromino(&game_info);

  if (original.type == TETROMINO_O) {
    for (int i = 0; i < 4; i++) {
      ck_assert_int_eq(game_info.current_tetromino.blocks[i].x, original.blocks[i].x);
      ck_assert_int_eq(game_info.current_tetromino.blocks[i].y, original.blocks[i].y);
    }
  }
}
END_TEST

START_TEST(test_rotate_tetromino_with_collision)
{
  GameInfo_t game_info = init_game_info();

  for (int i = 0; i < 4; i++) {
    int x = game_info.current_tetromino.blocks[i].x;
    int y = game_info.current_tetromino.blocks[i].y;
    if (x >= 0 && x < FIELD_WIDTH && y >= 0 && y < FIELD_HEIGHT) {
      game_info.field.cells[y][x] = 1;
    }
  }

  Tetromino original = game_info.current_tetromino;

  rotate_tetromino(&game_info);

  for (int i = 0; i < 4; i++) {
    ck_assert_int_eq(game_info.current_tetromino.blocks[i].x, original.blocks[i].x);
    ck_assert_int_eq(game_info.current_tetromino.blocks[i].y, original.blocks[i].y);
  }
}
END_TEST

Suite* backend_suite(void)
{
  Suite* s = suite_create("Backend");

  TCase* tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_handle_tetromino_fall);
  tcase_add_test(tc_core, test_handle_stick);
  tcase_add_test(tc_core, test_move_down);
  tcase_add_test(tc_core, test_move_down_with_collision);
  tcase_add_test(tc_core, test_generate_next_tetromino);
  tcase_add_test(tc_core, test_is_collision_below);
  tcase_add_test(tc_core, test_stick_to_bottom);
  tcase_add_test(tc_core, test_clear_full_lines);
  tcase_add_test(tc_core, test_generate_rand_tetromino);
  tcase_add_test(tc_core, test_check_game_over);
  tcase_add_test(tc_core, test_reset_game);
  tcase_add_test(tc_core, test_rotate_tetromino);
  tcase_add_test(tc_core, test_rotate_tetromino_with_collision);
  suite_add_tcase(s, tc_core);

  return s;
}
