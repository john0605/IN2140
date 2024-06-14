CFLAGS = -Wextra -Wall -g -std=gnu11
VFLAGS = --track-origins=yes --leak-check=full --malloc-fill=0x40 --free-fill=0x23 --leak-check=full --show-leak-kinds=all

make: tests.c stringoperations.c
	gcc ${CFLAGS} $^ -o ferdig_program

val: ferdig_program
	valgrind ${VFLAGS} -s ./$^ 

run: ferdig_program
	./$^

clean:
	rm ferdig_program
