OBJECTS = main.o
CC = gcc
CFLAGS = -Wall -lm -g
EXEC_NAME = prog

all: main.o
	$(CC) -o $(EXEC_NAME) main.o $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(EXEC_NAME)

