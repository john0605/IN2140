CFLAGS = -Wextra -Wall -g -std=gnu11
VFLAGS = --track-origins=yes --leak-check=full --malloc-fill=0x40 --free-fill=0x23

make: vowelshift.c
	gcc ${CFLAGS} $^ -o vowelshift
	
val: vowelshift
	valgrind ${VFLAGS} -s ./vowelshift "Lorem ipsum dolor sit amet" A

run: vowelshift
	./$^ "Lorem ipsum dolor sit amet" Q

clean:
	rm vowelshift
