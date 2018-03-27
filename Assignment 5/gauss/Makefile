OBJECTS = gauss.o gauss_serial.o gauss_openmp.o
CC = gcc
CFLAGS = -Wall -fopenmp
EXEC_NAME = gauss

all: gauss.o gauss_serial.o gauss_openmp.o
	$(CC) -o $(EXEC_NAME) gauss.o gauss_serial.o gauss_openmp.o $(CFLAGS)

gauss.o: gauss.c
	$(CC) -c gauss.c $(CFLAGS)

gauss_serial.o: gauss_serial.c
	$(CC) -c gauss_serial.c $(CFLAGS)

gauss_openmp.o: gauss_openmp.c
	$(CC) -c gauss_openmp.c $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(EXEC_NAME)

