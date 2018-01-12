#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Crossover and mutation rate should be in range 0 to 100 inclusive.
struct config {
	int max_crossovers_per_pair;
	int crossover_rate;
	int max_mutations_per_string_per_iteration;
	int mutation_rate;
	int pop_size;
	int string_size; // in bits
	float total_fitness; // changes on each iteration
};

struct config params;

struct individual {
	int string; // TODO: other sizes
	int num_ones;
	float fitness;
	int id;
};

struct individual *population;
struct individual *population_next;

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

void recalculate_fitness() {
	params.total_fitness = 0.0f;
	int i;
	for (i = 0; i < params.pop_size; i++) {
		struct individual *ind = &(population[i]);
		ind->num_ones = count_ones(ind);
		ind->fitness = (float)ind->num_ones / (float)params.string_size;
		params.total_fitness += ind->fitness;
	}
	params.total_fitness /= params.pop_size;
}

void init_population() {
	population = calloc(params.pop_size, sizeof(struct individual));
	population_next = calloc(params.pop_size, sizeof(struct individual));
	int i;
	for (i = 0; i < params.pop_size; i++) {
		struct individual *ind = &(population[i]);
		ind->id = i;
		ind->string = rand(); // TODO: other sizes
	}
	recalculate_fitness();
}

// TODO: probably need a better way of doing this
struct individual *select_individual() {
	while (1) {
		int i;
		int chance = rand() % 100;
		for (i = 0; i < params.pop_size; i++) {
			struct individual *ind = &(population[i]);
			float sel_chance = (ind->fitness / params.total_fitness) * 100.0f;
			if (sel_chance > chance) {
				//printf("Chance = %d, returing with fitness %f\n", chance, ind->fitness);
				return ind;
			}
		}
	}
}

void do_selection_stage() {
	int i;
	for (i = 0; i < params.pop_size; i++) {
		struct individual *sel = select_individual();
		population_next[i] = *sel;
	}
	struct individual *temp = population;
	population = population_next;
	population_next = temp;
}

// TODO: support for string size != 32
// TODO: more advanced crossover
void do_crossover_stage() {
	int i, n, j;
	for (i = 0; i < params.pop_size - 1; i++) {
		for (n = i + 1; n < params.pop_size; n++) {
			int chance = rand() % 100;
			if (chance >= params.crossover_rate) {
				int num_crossovers = (rand() % params.max_crossovers_per_pair) + 1;
				//printf("Num crossovers: %d\n", num_crossovers);
				//printf("Crossover %d and %d\n", i, n);
				for (j = 0; j < num_crossovers; j++) {
					int bit_num = rand() % params.string_size;
					int mask = pow(2, bit_num);
					int i_temp = population[i].string;
					population[i].string = population[i].string ^ (population[n].string & mask);
					population[n].string = population[n].string ^ (i_temp & mask);
				}
			}
		}
	}
}

// TODO: support for string size != 32
void do_mutation_stage() {
	int i, n;
	for (i = 0; i < params.pop_size; i++) {
		int chance = rand() % 100;
		if (params.mutation_rate >= chance) {
			int num_mutations = (rand() % params.max_mutations_per_string_per_iteration) + 1;
			for (n = 0; n < num_mutations; n++) {
				int bit_num = rand() % params.string_size;
				int mask = pow(2, bit_num);
				population[i].string = population[i].string ^ mask;
			}
		}
	}
}

void do_iteration() {
	do_selection_stage();
	//do_crossover_stage();
	do_mutation_stage();
	recalculate_fitness();
}

// TODO: read from args
void init() {
	srand(time(NULL));
	params.max_crossovers_per_pair = 3;
	params.crossover_rate = 60;
	params.max_mutations_per_string_per_iteration = 2;
	params.mutation_rate = 10;
	params.pop_size = 10;
	params.string_size = 32;
}

void print_state() {
	printf("===============================================\n");
	printf("Total fitness: %f\n", params.total_fitness);
	int i;
	for (i = 0; i < params.pop_size; i++) {
		printf("Individual %d num ones: %d\n", i, population[i].num_ones);
	}
	printf("===============================================\n");
}

int main(void) {
	init();
	init_population();
	print_state();
	int i;
	for (i = 0; i < 10; i++) {
		do_iteration();
		//print_state();
	}
	print_state();
	return 0;
}
