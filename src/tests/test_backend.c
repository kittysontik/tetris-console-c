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

START_TEST(test_is_collision_on_sides_no_collision)
{
  GameInfo_t game_info = {0};

  for (int i = 0; i < 4; i++) {
    game_info.current_tetromino.blocks[i].x = 4 + i;
    game_info.current_tetromino.blocks[i].y = 10;
  }
  ck_assert(!is_collision_on_sides(&game_info, 1));
  ck_assert(!is_collision_on_sides(&game_info, -1));
}
END_TEST

START_TEST(test_is_collision_on_sides_border_collision)
{
  GameInfo_t game_info = {0};

  for (int i = 0; i < 4; i++) {
    game_info.current_tetromino.blocks[i].x = FIELD_WIDTH - 1;
    game_info.current_tetromino.blocks[i].y = i;
  }
  ck_assert(is_collision_on_sides(&game_info, 1));
}
END_TEST

START_TEST(test_is_collision_on_sides_cell_occupied)
{
  GameInfo_t game_info = {0};

  game_info.field.cells[10][5] = 1;
  for (int i = 0; i < 4; i++) {
    game_info.current_tetromino.blocks[i].x = 4;
    game_info.current_tetromino.blocks[i].y = 10;
  }
  ck_assert(is_collision_on_sides(&game_info, 1));
}
END_TEST

START_TEST(test_move_right_success)
{
  GameInfo_t game_info = {0};

  for (int i = 0; i < 4; i++) {
    game_info.current_tetromino.blocks[i].x = 5;
    game_info.current_tetromino.blocks[i].y = 10;
  }
  move_right(&game_info, 1);
  for (int i = 0; i < 4; i++) {
    ck_assert_int_eq(game_info.current_tetromino.blocks[i].x, 6);
  }
}
END_TEST

START_TEST(test_move_right_blocked)
{
  GameInfo_t game_info = {0};

  for (int i = 0; i < 4; i++) {
    game_info.current_tetromino.blocks[i].x = FIELD_WIDTH - 1;
    game_info.current_tetromino.blocks[i].y = 10;
  }
  move_right(&game_info, 1);
  for (int i = 0; i < 4; i++) {
    ck_assert_int_eq(game_info.current_tetromino.blocks[i].x, FIELD_WIDTH - 1);
  }
}
END_TEST

START_TEST(test_move_left_success)
{
  GameInfo_t game_info = {0};

  for (int i = 0; i < 4; i++) {
    game_info.current_tetromino.blocks[i].x = 5;
    game_info.current_tetromino.blocks[i].y = 10;
  }
  move_left(&game_info, -1);
  for (int i = 0; i < 4; i++) {
    ck_assert_int_eq(game_info.current_tetromino.blocks[i].x, 4);
  }
}
END_TEST

START_TEST(test_move_left_blocked)
{
  GameInfo_t game_info = {0};

  for (int i = 0; i < 4; i++) {
    game_info.current_tetromino.blocks[i].x = 0;
    game_info.current_tetromino.blocks[i].y = 10;
  }
  move_left(&game_info, -1);
  for (int i = 0; i < 4; i++) {
    ck_assert_int_eq(game_info.current_tetromino.blocks[i].x, 0);
  }
}
END_TEST

START_TEST(test_calculate_score)
{
  ck_assert_int_eq(calculate_score(1), 100);
  ck_assert_int_eq(calculate_score(2), 300);
  ck_assert_int_eq(calculate_score(3), 700);
  ck_assert_int_eq(calculate_score(4), 1500);
  ck_assert_int_eq(calculate_score(0), 0);
  ck_assert_int_eq(calculate_score(5), 0);
}
END_TEST

START_TEST(test_clear_full_lines_clears_one_line)
{
  GameInfo_t game_info = {0};
  for (int i = 0; i < FIELD_WIDTH; i++) {
    game_info.field.cells[5][i] = 1;
  }
  int lines_cleared = clear_full_lines(&game_info);
  ck_assert_int_eq(lines_cleared, 1);
  for (int i = 0; i < FIELD_WIDTH; i++) {
    ck_assert_int_eq(game_info.field.cells[5][i], 0);
  }
}
END_TEST

START_TEST(test_update_level_increase)
{
  GameInfo_t game_info = {0};
  game_info.score = 600;
  update_level(&game_info);
  ck_assert_int_eq(game_info.level, 1);
}
END_TEST

START_TEST(test_update_level_limits_to_max)
{
  GameInfo_t game_info = {0};
  game_info.score = 10000;
  update_level(&game_info);
  ck_assert_int_eq(game_info.level, 9);
}
END_TEST

START_TEST(test_update_speed_basic)
{
  GameInfo_t game_info = {0};
  game_info.level = 0;
  update_speed(&game_info);
  ck_assert_int_eq(game_info.speed, 500000);
}
END_TEST

START_TEST(test_update_speed_after_levelup)
{
  GameInfo_t game_info = {0};
  game_info.level = 5;
  update_speed(&game_info);
  ck_assert_int_eq(game_info.speed, 400000);
}
END_TEST

START_TEST(test_update_speed_limits_to_max_speed)
{
  GameInfo_t game_info = {0};
  game_info.level = 9;
  update_speed(&game_info);
  ck_assert_int_eq(game_info.speed, 320000);
}
END_TEST

START_TEST(test_handle_user_input_terminate)
{
  GameInfo_t game_info = {.game_state = STATE_PLAYING};
  handle_user_input(Terminate, &game_info, NULL);
  ck_assert_int_eq(game_info.game_state, STATE_EXIT);
}
END_TEST

START_TEST(test_handle_user_input_pause_start)
{
  GameInfo_t game_info = {.pause = false};
  handle_user_input(Pause, &game_info, NULL);
  ck_assert_int_eq(game_info.pause, true);
  handle_user_input(Start, &game_info, NULL);
  ck_assert_int_eq(game_info.pause, false);
}
END_TEST

START_TEST(test_handle_user_input_movement_when_paused)
{
  GameInfo_t game_info = {.pause = true};
  handle_user_input(Down, &game_info, NULL);
  handle_user_input(Left, &game_info, NULL);
  handle_user_input(Right, &game_info, NULL);
  handle_user_input(Action, &game_info, NULL);
}
END_TEST

START_TEST(test_handle_input_if_any_no_action)
{
  GameInfo_t game_info = {0};
  GameWindows windows = {0};
  windows.game_win = newwin(10, 10, 0, 0);
  wtimeout(windows.game_win, 1);
  wgetch(windows.game_win);
  handle_input_if_any(&game_info, &windows);
  delwin(windows.game_win);
}
END_TEST

START_TEST(test_handle_state_playing)
{
  GameInfo_t game_info = {.game_state = STATE_PLAYING};
  GameWindows windows = {0};
  handle_state_playing(&game_info, &windows);
  ck_assert_int_eq(game_info.game_state, STATE_PLAYING);
}
END_TEST

START_TEST(test_run_game_fsm_exit)
{
  GameInfo_t game_info = {.game_state = STATE_EXIT};
  GameWindows windows = {0};
  run_game_fsm(&game_info, &windows);
  ck_assert_int_eq(game_info.game_state, STATE_EXIT);
}
END_TEST
START_TEST(test_check_game_over_true)
{
  GameInfo_t game_info = init_game_info();
  game_info.game_state = STATE_PLAYING;

  game_info.field.cells[0][0] = 1;

  game_info.current_tetromino.blocks[0].x = 0;
  game_info.current_tetromino.blocks[0].y = 0;

  for (int i = 1; i < 4; i++) {
    game_info.current_tetromino.blocks[i].x = -1;
    game_info.current_tetromino.blocks[i].y = -1;
  }

  check_game_over(&game_info);

  ck_assert_int_eq(game_info.game_state, STATE_GAME_OVER);
}
END_TEST

START_TEST(test_check_game_over_false)
{
  GameInfo_t game_info = {.game_state = STATE_PLAYING};

  memset(game_info.field.cells, 0, sizeof(game_info.field.cells));

  for (int i = 0; i < 4; i++) {
    game_info.current_tetromino.blocks[i].x = i;
    game_info.current_tetromino.blocks[i].y = 0;
  }

  check_game_over(&game_info);

  ck_assert_int_eq(game_info.game_state, STATE_PLAYING);
}
END_TEST

START_TEST(test_handle_state_game_over)
{
  GameInfo_t game_info = {.game_state = STATE_GAME_OVER};
  GameWindows windows = {0};
  handle_state_game_over(&game_info, &windows);
  ck_assert_int_eq(game_info.game_state, STATE_EXIT);
}
END_TEST

START_TEST(test_map_key_to_action)
{
  ck_assert_int_eq(map_key_to_action('q'), Terminate);
  ck_assert_int_eq(map_key_to_action('p'), Pause);
  ck_assert_int_eq(map_key_to_action('s'), Start);
  ck_assert_int_eq(map_key_to_action(KEY_LEFT), Left);
  ck_assert_int_eq(map_key_to_action(KEY_RIGHT), Right);
  ck_assert_int_eq(map_key_to_action(KEY_DOWN), Down);
  ck_assert_int_eq(map_key_to_action(KEY_UP), Action);
  ck_assert_int_eq(map_key_to_action('x'), NoAction);
}
END_TEST

START_TEST(test_map_key_to_action_terminate)
{
  ck_assert_int_eq(map_key_to_action('q'), Terminate);
}
END_TEST

START_TEST(test_map_key_to_action_pause)
{
  ck_assert_int_eq(map_key_to_action('p'), Pause);
}
END_TEST

START_TEST(test_map_key_to_action_start)
{
  ck_assert_int_eq(map_key_to_action('s'), Start);
}
END_TEST

START_TEST(test_map_key_to_action_left)
{
  ck_assert_int_eq(map_key_to_action(KEY_LEFT), Left);
}
END_TEST

START_TEST(test_map_key_to_action_right)
{
  ck_assert_int_eq(map_key_to_action(KEY_RIGHT), Right);
}
END_TEST

START_TEST(test_map_key_to_action_down)
{
  ck_assert_int_eq(map_key_to_action(KEY_DOWN), Down);
}
END_TEST

START_TEST(test_map_key_to_action_action)
{
  ck_assert_int_eq(map_key_to_action(KEY_UP), Action);
}
END_TEST

START_TEST(test_map_key_to_action_no_action)
{
  ck_assert_int_eq(map_key_to_action('x'), NoAction);
}
END_TEST

START_TEST(test_handle_user_input_terminate_action)
{
  GameInfo_t game_info = {.game_state = STATE_MENU};
  GameWindows game_windows;

  handle_user_input(Terminate, &game_info, &game_windows);

  ck_assert_int_eq(game_info.game_state, STATE_EXIT);
}
END_TEST

START_TEST(test_handle_user_input_pause_action)
{
  GameInfo_t game_info = {.pause = false, .game_state = STATE_MENU};
  GameWindows game_windows;

  handle_user_input(Pause, &game_info, &game_windows);

  ck_assert_int_eq(game_info.pause, true);
}
END_TEST

START_TEST(test_handle_user_input_start_action)
{
  GameInfo_t game_info = {.pause = false, .game_state = STATE_MENU};
  GameWindows game_windows;

  handle_user_input(Start, &game_info, &game_windows);

  ck_assert_int_eq(game_info.pause, true);
}
END_TEST

START_TEST(test_handle_user_input_down_action_when_paused)
{
  GameInfo_t game_info = {.pause = true, .game_state = STATE_MENU};
  GameWindows game_windows;

  handle_user_input(Down, &game_info, &game_windows);

  ck_assert_int_eq(game_info.game_state, STATE_MENU);
}
END_TEST

START_TEST(test_handle_user_input_right_action_when_active)
{
  GameInfo_t game_info = {.pause = false, .game_state = STATE_MENU};
  GameWindows game_windows;

  handle_user_input(Right, &game_info, &game_windows);

  ck_assert_int_eq(game_info.game_state, STATE_MENU);
}
END_TEST

START_TEST(test_handle_user_input_left_action_when_active)
{
  GameInfo_t game_info = {.pause = false, .game_state = STATE_MENU};
  GameWindows game_windows;

  handle_user_input(Left, &game_info, &game_windows);

  ck_assert_int_eq(game_info.game_state, STATE_MENU);
}
END_TEST

START_TEST(test_handle_user_input_rotate_action_when_active)
{
  GameInfo_t game_info = {.pause = false, .game_state = STATE_MENU};
  GameWindows game_windows;

  handle_user_input(Action, &game_info, &game_windows);

  ck_assert_int_eq(game_info.game_state, STATE_MENU);
}
END_TEST

START_TEST(test_handle_user_input_unknown_key_action)
{
  GameInfo_t game_info = {.game_state = STATE_MENU};
  GameWindows game_windows;

  handle_user_input(99, &game_info, &game_windows);
  ck_assert_int_eq(game_info.game_state, STATE_MENU);
}
END_TEST

START_TEST(test_handle_full_lines_no_full_lines)
{
  GameInfo_t game_info;
  memset(&game_info, 0, sizeof(game_info));

  handle_full_lines(&game_info);

  ck_assert_int_eq(has_full_rows(&game_info.field), 0);
}
END_TEST

START_TEST(test_handle_full_lines_multiple_lines_partial)
{
  GameInfo_t game_info;
  memset(&game_info, 0, sizeof(game_info));

  for (int i = 0; i < FIELD_WIDTH; i++) {
    game_info.field.cells[18][i] = 1;
  }

  handle_full_lines(&game_info);

  for (int i = 0; i < FIELD_WIDTH; i++) {
    ck_assert_int_eq(game_info.field.cells[19][i], 0);
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
  tcase_add_test(tc_core, test_check_game_over);
  tcase_add_test(tc_core, test_reset_game);
  tcase_add_test(tc_core, test_rotate_tetromino);
  tcase_add_test(tc_core, test_rotate_tetromino_with_collision);
  tcase_add_test(tc_core, test_is_collision_on_sides_no_collision);
  tcase_add_test(tc_core, test_is_collision_on_sides_border_collision);
  tcase_add_test(tc_core, test_is_collision_on_sides_cell_occupied);
  tcase_add_test(tc_core, test_move_right_success);
  tcase_add_test(tc_core, test_move_right_blocked);
  tcase_add_test(tc_core, test_move_left_success);
  tcase_add_test(tc_core, test_move_left_blocked);
  tcase_add_test(tc_core, test_calculate_score);
  tcase_add_test(tc_core, test_update_level_limits_to_max);
  tcase_add_test(tc_core, test_update_speed_limits_to_max_speed);
  tcase_add_test(tc_core, test_clear_full_lines_clears_one_line);
  tcase_add_test(tc_core, test_update_level_increase);
  tcase_add_test(tc_core, test_update_speed_basic);
  tcase_add_test(tc_core, test_update_speed_after_levelup);
  tcase_add_test(tc_core, test_handle_user_input_terminate);
  tcase_add_test(tc_core, test_handle_user_input_pause_start);
  tcase_add_test(tc_core, test_handle_user_input_movement_when_paused);
  tcase_add_test(tc_core, test_handle_input_if_any_no_action);
  tcase_add_test(tc_core, test_handle_state_playing);
  tcase_add_test(tc_core, test_run_game_fsm_exit);
  tcase_add_test(tc_core, test_check_game_over_true);
  tcase_add_test(tc_core, test_check_game_over_false);
  tcase_add_test(tc_core, test_handle_state_game_over);
  tcase_add_test(tc_core, test_map_key_to_action);
  tcase_add_test(tc_core, test_map_key_to_action_terminate);
  tcase_add_test(tc_core, test_map_key_to_action_pause);
  tcase_add_test(tc_core, test_map_key_to_action_start);
  tcase_add_test(tc_core, test_map_key_to_action_left);
  tcase_add_test(tc_core, test_map_key_to_action_right);
  tcase_add_test(tc_core, test_map_key_to_action_down);
  tcase_add_test(tc_core, test_map_key_to_action_action);
  tcase_add_test(tc_core, test_map_key_to_action_no_action);
  tcase_add_test(tc_core, test_handle_user_input_terminate_action);
  tcase_add_test(tc_core, test_handle_user_input_pause_action);
  tcase_add_test(tc_core, test_handle_user_input_start_action);
  tcase_add_test(tc_core, test_handle_user_input_down_action_when_paused);
  tcase_add_test(tc_core, test_handle_user_input_right_action_when_active);
  tcase_add_test(tc_core, test_handle_user_input_left_action_when_active);
  tcase_add_test(tc_core, test_handle_user_input_rotate_action_when_active);
  tcase_add_test(tc_core, test_handle_user_input_unknown_key_action);
  tcase_add_test(tc_core, test_handle_full_lines_no_full_lines);
  tcase_add_test(tc_core, test_handle_full_lines_multiple_lines_partial);

  suite_add_tcase(s, tc_core);

  return s;
}
