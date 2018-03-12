OBJECTS = sieve.o
CC = gcc
CFLAGS = -Wall -lm -fopenmp
EXEC_NAME = sieve

all: sieve.o
	$(CC) -o $(EXEC_NAME) sieve.o $(CFLAGS)

sieve.o: sieve.c
	$(CC) -c sieve.c $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(EXEC_NAME)

