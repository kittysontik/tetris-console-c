CC := gcc
CFLAGS := -I inc -std=c99 -Wall -Werror -Wpedantic
OBJS_FSMSWITCHCASE := out/frogger.o out/fsm.o out/backend.o out/frontend.o
OBJS_FSMTABLE := out/frogger.o out/fsm_matrix.o out/backend.o out/frontend.o

CLANG-FORMAT = clang-format -style='{CommentPragmas: Insert, BasedOnStyle: Google}'

frogger: $(OBJS_FSMSWITCHCASE)
	$(CC) $(CFLAGS) $^ -o $@ -lncurses

frogger_fsmtable: $(OBJS_FSMTABLE)
	$(CC) $(CFLAGS) $^ -o $@ -lncurses

out/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) out/*.o frogger frogger_fsmtable


check_style:
	find . -name "*.h" -or -name "*.c" | xargs $(CLANG-FORMAT) -n
format:
	find . -name "*.h" -or -name "*.c" | xargs $(CLANG-FORMAT) -i