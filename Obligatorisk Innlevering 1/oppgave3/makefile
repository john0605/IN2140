CFLAGS = -Wextra -Wall -g -std=gnu11
VFLAGS = --track-origins=yes --leak-check=full --malloc-fill=0x40 --free-fill=0x23

make: apple.c
	gcc ${CFLAGS} $^ -o apple

run: apple
	./$^

val: apple
	valgrind ${VFLAGS} ./$^

clean:
	rm apple
