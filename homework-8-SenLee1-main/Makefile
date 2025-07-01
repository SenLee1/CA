CC = gcc
CFLAGS = -g -std=c11 -Wpedantic -Wall -Wextra -Werror -Wvla -O0
MEMCHECK=valgrind --tool=memcheck --leak-check=full --track-origins=yes

all: 
	$(CC) $(CFLAGS) -o vm_tlb main.c src/simulator.c src/memory.c src/process.c src/TLB.c

memcheck: all
	$(MEMCHECK) ./vm_tlb REPLACE_ME_WITH_FILENAME

test: all
	./vm_tlb REPLACE_ME_WITH_FILENAME

clean:
	rm -f ./vm_tlb
