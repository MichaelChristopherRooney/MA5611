OBJECTS = parallel.o grid.o
CC = mpicc
CFLAGS = -Wall -lm
EXEC_NAME = prog

all: parallel.o grid.o
	$(CC) -o $(EXEC_NAME) parallel.o grid.o $(CFLAGS)

parallel.o: parallel.c common.h
	$(CC) -c parallel.c $(CFLAGS)

grid.o: grid.c common.h
	$(CC) -c grid.c $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(EXEC_NAME)

