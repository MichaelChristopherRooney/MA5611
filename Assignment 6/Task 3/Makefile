OBJECTS = main.o city.o
CC = gcc
CFLAGS = -Wall -fopenmp -lm
EXEC_NAME = prog

all: main.o city.o
	$(CC) -o $(EXEC_NAME) main.o city.o $(CFLAGS)

main.o: main.c common.h
	$(CC) -c main.c $(CFLAGS)

city.o: city.c common.h
	$(CC) -c city.c $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(EXEC_NAME)

