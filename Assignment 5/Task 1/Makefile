OBJECTS = gauss.o
CC = gcc
CFLAGS = -Wall
EXEC_NAME = gauss

all: gauss.o
	$(CC) -o $(EXEC_NAME) gauss.o $(CFLAGS)

gauss.o: gauss.c
	$(CC) -c gauss.c $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(EXEC_NAME)

