OBJECTS = main.o genetic.o pd.o
CC = gcc
CFLAGS = -Wall -lm
EXEC_NAME = prog

all: main.o genetic.o pd.o
	$(CC) -o $(EXEC_NAME) main.o genetic.o pd.o $(CFLAGS)

main.o: main.c common.h
	$(CC) -c main.c $(CFLAGS)

pd.o: pd.c common.h
	$(CC) -c pd.c $(CFLAGS)

genetic.o: genetic.c common.h
	$(CC) -c genetic.c $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(EXEC_NAME)

