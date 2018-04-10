OBJECTS = main.o city.o greedy_solver.o 2opt_solver.o
CC = gcc
CFLAGS = -Wall -fopenmp -lm
EXEC_NAME = prog

all: main.o city.o greedy_solver.o 2opt_solver.o
	$(CC) -o $(EXEC_NAME) main.o city.o greedy_solver.o 2opt_solver.o $(CFLAGS)

main.o: main.c common.h
	$(CC) -c main.c $(CFLAGS)

city.o: city.c common.h
	$(CC) -c city.c $(CFLAGS)

greedy_solver.o: greedy_solver.c common.h
	$(CC) -c greedy_solver.c $(CFLAGS)

2opt_solver.o: 2opt_solver.c common.h
	$(CC) -c 2opt_solver.c $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(EXEC_NAME)

