CC = gcc
CF = -Wall -Werror

all: first

first: first.o
	$(CC) $(CF) first.o -o first -lm

first.o: first.c
	$(CC) $(CF) -c first.c

clean:
	rm -f *.o
	rm -f first