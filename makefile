CC = clang
CFLAGS = -std=c99 -O2 -Wall -Wextra -Wno-implicit-function-declaration

.PHONY:
.DEFAULT: notty

notty: notty.o
	$(CC) $< -o $@
	strip $@

notty.o: notty.c
	$(CC) -c $< -o $@ $(CFLAGS)
