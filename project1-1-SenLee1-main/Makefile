CC = gcc
CFLAGS = -g -std=c11 -Wpedantic -Wall -Wextra -Werror -Wvla
MEMCHECK=valgrind --tool=memcheck --leak-check=full --track-origins=yes

all: 
	$(CC) $(CFLAGS) -o main main.c src/assembler.c src/util.c

memcheck: all
	$(MEMCHECK) ./main ./test/test.S ./test/test.out

test: all
	./main ./test/test.S ./test/test.out
test0:all
	./main ./test/test0.S ./test/test.out
clean:
	rm -f ./main
