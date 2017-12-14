objects = mat_mul_part_parallel.o print_functions.o mat_util.o timing.o
CC = mpicc
CFLAGS = -Wall -lm
EXEC_NAME = prog

all: $(objects)
	$(CC) -o $(EXEC_NAME) $(objects) $(CFLAGS)

mat_mul_part_parallel.o: mat_mul_part_parallel.c common.h
	$(CC) -c mat_mul_part_parallel.c $(CFLAGS)

print_functions.o: print_functions.c common.h
	$(CC) -c print_functions.c $(CFLAGS)

mat_util.o: mat_util.c common.h
	$(CC) -c mat_util.c $(CFLAGS)

timing.o: timing.c common.h
	$(CC) -c timing.c $(CFLAGS)

clean:
	rm -f $(objects) $(EXEC_NAME)
