CC = gcc
CFLAGS = -g -std=c11 -Wpedantic -Wall -Wextra -Werror -Wvla
MEMCHECK=valgrind --tool=memcheck --leak-check=full --track-origins=yes

all:
	$(CC) $(CFLAGS) -o main *.c

memcheck: all
	$(MEMCHECK) ./main input.txt C_program.out

clean:
	rm -f ./main *.out