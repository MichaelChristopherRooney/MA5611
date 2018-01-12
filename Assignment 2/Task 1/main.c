#include <stdio.h>
#include <stdlib.h>

struct config {
	int mutation_rate; // % as 0-100
	int pop_size;
};

struct config params;

struct individual {
	int string; // TODO: other sizes
	int num_ones;
	int id;
};

struct individual *population;

int pop_size;

// TODO: other sizes
// TODO: use faster method such as power of two
// n = n & (n-1);
// count++;
int count_ones(struct individual *ind) {
	int mask = 1;
	int count = 0;
	int copy = ind->string;
	int i;
	for (i = 0; i < 32; i++) {
		count += copy & mask;
		copy = copy >> 1;
	}
	return count;
}

void init_population() {
	population = calloc(pop_size, sizeof(struct individual));
	int i;
	for (i = 0; i < pop_size; i++) {
		struct individual *ind = &(population[i]);
		ind->id = i;
		ind->string = rand(); // TODO: other sizes
		ind->num_ones = count_ones(ind);
		printf("1s in string %d: %d\n", ind->string, ind->num_ones);
	}
}

// TODO: read from args
int init() {
	params.mutation_rate = 10;
	params.pop_size = 4;
}

int main(void) {
	init_population();
	return 0;
}
