CC = gcc
CFLAGS = -g -Wpedantic -Wall -Wextra -Wvla -Werror -std=c11

.PHONY: all build test clean memcheck

all: test

FloatCalculate:
	gcc test.c FloatCalculate.c $(CFLAGS) -o FloatCalculate -lm

build: FloatCalculate

test: build
	./FloatCalculate

clean:
	rm -f FloatCalculate

memcheck: build
	valgrind --tool=memcheck --leak-check=full --track-origins=yes ./FloatCalculate