#include <check.h>

#include "../tetris.h"

START_TEST(test_handle_tetromino_fall)
{
  GameInfo_t game_info = init_game_info();
  game_info.pause = false;
  game_info.speed = 1;

  // Сохраняем начальное положение
  int initial_y = game_info.current_tetromino.blocks[0].y;

  // Настраиваем время, чтобы elapsed_time >= speed
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

  // Опускаем фигуру до самого низа вручную
  for (int i = 0; i < 4; i++) {
    game_info.current_tetromino.blocks[i].y = FIELD_HEIGHT - 1;
  }

  // Теперь handle_stick должен её "приклеить"
  bool stuck = handle_stick(&game_info, NULL);
  ck_assert(stuck);

  // Проверим, что после прилипания появилась новая фигура на верху
  for (int i = 0; i < 4; i++) {
    ck_assert_int_lt(game_info.current_tetromino.blocks[i].y, FIELD_HEIGHT / 2);
  }
}
END_TEST

START_TEST(test_move_down)
{
  GameInfo_t game_info = init_game_info();

  int initial_y = game_info.current_tetromino.blocks[0].y;

  // Двигаем тетромино вниз
  move_down(&game_info);

  // Проверяем, что тетромино переместилось вниз
  ck_assert_int_gt(game_info.current_tetromino.blocks[0].y, initial_y);
}
END_TEST

START_TEST(test_move_down_with_collision)
{
  GameInfo_t game_info = init_game_info();

  // Симулируем, что тетромино столкнулось с другим
  game_info.current_tetromino.blocks[0].y = FIELD_HEIGHT - 1;

  // Пробуем двигать вниз
  move_down(&game_info);

  // Проверяем, что тетромино не сдвинулось
  ck_assert_int_eq(game_info.current_tetromino.blocks[0].y, FIELD_HEIGHT - 1);
}
END_TEST

START_TEST(test_generate_next_tetromino)
{
  GameInfo_t game_info = init_game_info();

  // Проверяем, что текущее тетромино и следующее — разные
  Tetromino current_tetromino = game_info.current_tetromino;
  generate_next_tetromino(&game_info);

  // Проверяем, что новое тетромино отличается от текущего
  ck_assert_ptr_ne(&game_info.current_tetromino, &current_tetromino);
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
  suite_add_tcase(s, tc_core);

  return s;
}
