OBJECTS = serial.o tree.o
CC = gcc
CFLAGS = -Wall -lm
PARALLEL_EXEC_NAME = parallel
SERIAL_EXEC_NAME = serial

all: serial-tree

serial-tree: serial.o tree.o
	$(CC) -o $(SERIAL_EXEC_NAME) serial.o tree.o $(CFLAGS)

serial.o: serial.c tree.h
	$(CC) -c serial.c $(CFLAGS)

tree.o: tree.c tree.h
	$(CC) -c tree.c $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(SERIAL_EXEC_NAME) $(PARALLEL_EXEC_NAME)

