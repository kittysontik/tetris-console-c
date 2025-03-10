CC = gcc
CFLAGS = -I inc -std=c11 -Wall -Werror -Wpedantic
OBJS = out/*.o
LIB_FLAG = -lncurses

CLANG-FORMAT = clang-format -style='{CommentPragmas: Insert, BasedOnStyle: Google}'

tetris: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIB_FLAG)

out/%.o: gui/cli/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) out/*.o *.out tetris


check_style:
	find . -name "*.h" -or -name "*.c" | xargs $(CLANG-FORMAT) -n
format:
	find . -name "*.h" -or -name "*.c" | xargs $(CLANG-FORMAT) -i