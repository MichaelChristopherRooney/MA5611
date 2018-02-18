OBJECTS = serial.o tree.o parallel.o
CC = gcc
CFLAGS = -Wall -lm -lpthread
PARALLEL_EXEC_NAME = parallel
SERIAL_EXEC_NAME = serial

all: serial-tree parallel-tree

parallel-tree: parallel.o tree.o
	$(CC) -o $(PARALLEL_EXEC_NAME) parallel.o tree.o $(CFLAGS)

parallel.o: parallel.c tree.h
	$(CC) -c parallel.c $(CFLAGS)

serial-tree: serial.o tree.o
	$(CC) -o $(SERIAL_EXEC_NAME) serial.o tree.o $(CFLAGS)

serial.o: serial.c tree.h
	$(CC) -c serial.c $(CFLAGS)

tree.o: tree.c tree.h
	$(CC) -c tree.c $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(SERIAL_EXEC_NAME) $(PARALLEL_EXEC_NAME)

