CC = clang
CFLAGS = -std=c99 -O2 -Wall -Wextra -Wno-implicit-function-declaration

OBJ = $(patsubst src/%.c, obj/%.o, $(wildcard src/*.c))

.PHONY:
.DEFAULT: bin/notty

bin/notty: $(OBJ)
	mkdir -p $(dir $@)
	$(CC) $(wildcard obj/*.o) -o $@

obj/%.o: src/%.c $(wildcard src/*.h)
	mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS)
