OUT_DIR = out

TESTS_DIR = tests

CC = gcc
CFLAGS = -std=c11 -Wall -Werror -Wextra -Wpedantic -Wno-gnu-zero-variadic-macro-arguments

LIBS = -lncurses
CHECK_LIBS = $(shell pkg-config --libs --cflags check)
COVERAGE_FLAGS = -fprofile-arcs -ftest-coverage

OBJS = $(OUT_DIR)/backend.o $(OUT_DIR)/tetris.o $(OUT_DIR)/frontend.o

TEST_OBJS = $(OUT_DIR)/test_backend.o $(OUT_DIR)/test_main.o

all: tetris

tetris: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	./tetris

$(OUT_DIR)/backend.o: brick_game/tetris/backend.c
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/tetris.o: brick_game/tetris/tetris.c
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/frontend.o: gui/cli/frontend.c
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


test: $(TEST_OBJS) $(OUT_DIR)/backend.o $(OUT_DIR)/frontend.o
	$(CC) $(CFLAGS) -o test_runner $^ $(CHECK_LIBS) $(LIBS)
	./test_runner

$(OUT_DIR)/%.o: $(TESTS_DIR)/%.c
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

coverage_build: CFLAGS += $(COVERAGE_FLAGS)
coverage_build: clean
	$(MAKE) test

gcov_report: coverage_build
	gcov -o out brick_game/tetris/backend.c


clean:
	rm -rf $(OUT_DIR) tetris test_runner


CLANG-FORMAT = clang-format

check_style:
	find . -name "*.c" -or -name "*.h" | xargs $(CLANG-FORMAT) -n

format:
	find . -name "*.c" -or -name "*.h" | xargs $(CLANG-FORMAT) -i

.PHONY: all test clean tetris

