OUT_DIR = out
TESTS_DIR = tests
INSTALL_DIR = /usr/local/bin

CC = gcc
CFLAGS = -std=c11 -Wall -Werror -Wextra -Wpedantic -Wno-gnu-zero-variadic-macro-arguments -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=500
LIBS = -lncurses 
CHECK_LIBS = $(shell pkg-config --libs --cflags check)
COVERAGE_FLAGS = -fprofile-arcs -ftest-coverage

OBJS = $(OUT_DIR)/backend.o $(OUT_DIR)/tetris.o $(OUT_DIR)/frontend.o
TEST_OBJS = $(OUT_DIR)/test_backend.o $(OUT_DIR)/test_main.o

all: tetris

tetris: $(OBJS)
	$(CC) $(CFLAGS) $(COVERAGE_FLAGS) -o $@ $^ $(LIBS)
	./tetris

$(OUT_DIR)/backend.o: brick_game/tetris/backend.c
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) $(COVERAGE_FLAGS) -c $< -o $@

$(OUT_DIR)/tetris.o: brick_game/tetris/tetris.c
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) $(COVERAGE_FLAGS) -c $< -o $@

$(OUT_DIR)/frontend.o: gui/cli/frontend.c
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) $(COVERAGE_FLAGS) -c $< -o $@

$(OUT_DIR)/%.o: $(TESTS_DIR)/%.c
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) $(COVERAGE_FLAGS) -c $< -o $@

test: $(TEST_OBJS) $(OUT_DIR)/backend.o $(OUT_DIR)/frontend.o
	$(CC) $(CFLAGS) $(COVERAGE_FLAGS) -o test_runner $^ $(CHECK_LIBS) $(LIBS)
	./test_runner

coverage_build: export CFLAGS := $(CFLAGS) $(COVERAGE_FLAGS)
coverage_build: clean
	$(MAKE) test

gcov_report: coverage_build
	@mkdir -p coverage
	gcovr -r . --html --html-details -o coverage/index.html --exclude $(TESTS_DIR) --exclude gui/cli/
	open coverage/index.html

install: tetris
	@mkdir -p $(INSTALL_DIR)
	cp tetris $(INSTALL_DIR)

uninstall:
	rm -f $(INSTALL_DIR)/tetris

dist:
	@mkdir -p dist
	tar -czf dist/tetris.tar.gz --exclude=dist --exclude=out --exclude=coverage --exclude=test_runner --exclude=*.gcda --exclude=*.gcno .

dvi:
	@echo "No DVI documentation to build."

clean:
	rm -rf $(OUT_DIR) tetris test_runner *.gcda *.gcno *.gcov coverage  dist/tetris.tar.gz

CLANG-FORMAT = clang-format

check_style:
	find . -name "*.c" -or -name "*.h" | xargs $(CLANG-FORMAT) -n

format:
	find . -name "*.c" -or -name "*.h" | xargs $(CLANG-FORMAT) -i

.PHONY: all tetris test clean coverage_build gcov_report check_style format install uninstall dist
