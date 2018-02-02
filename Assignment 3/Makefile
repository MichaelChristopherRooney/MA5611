OBJECTS = parallel.o grid.o serial_compare.o serial.o communication.o
CC = mpicc
CFLAGS = -Wall -lm
PARALLEL_EXEC_NAME = parallel
SERIAL_EXEC_NAME = serial

all: parallel serial

parallel: parallel.o grid.o serial_compare.o communication.o
	$(CC) -o $(PARALLEL_EXEC_NAME) parallel.o grid.o serial_compare.o communication.o $(CFLAGS)

serial: serial.o
	$(CC) -o $(SERIAL_EXEC_NAME) serial.o $(CFLAGS)

serial.o: serial.c common.h
	$(CC) -c serial.c $(CFLAGS)

parallel.o: parallel.c common.h
	$(CC) -c parallel.c $(CFLAGS)

communication.o: communication.c common.h
	$(CC) -c communication.c $(CFLAGS)

grid.o: grid.c common.h
	$(CC) -c grid.c $(CFLAGS)

serial_compare.o: serial_compare.c common.h
	$(CC) -c serial_compare.c $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(SERIAL_EXEC_NAME) $(PARALLEL_EXEC_NAME)

