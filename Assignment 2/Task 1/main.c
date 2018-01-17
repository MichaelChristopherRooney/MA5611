#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// TODO:
// number of bits than is not divisible by 32

#define NUM_BITS_IN_INT sizeof(int) * 8
// Crossover and mutation rate should be in range [0, 1]
struct config {
	float crossover_rate;
	float mutation_rate;
	int pop_size;
	int string_size; // in bits
	int string_size_in_ints; // string_size / 32
	float total_fitness_percent; // changes on each iteration
	int highest_fitness;
	int fitness_sum;
};

struct config params;

struct individual {
	int *string; // TODO: other sizes
	int fitness;
	int id;
};

struct individual *population;
struct individual *population_next;

// TODO: use faster method such as power of two
// n = n & (n-1);
// count++;
int count_ones(struct individual *ind) {
	int mask = 1;
	int count = 0;
	int i, n;
	for (i = 0; i < params.string_size_in_ints; i++) {
		int copy = ind->string[i];
		for (n = 0; n < NUM_BITS_IN_INT; n++) {
			count += copy & mask;
			copy = copy >> 1;
		}
	}
	return count;
}

void recalculate_fitness() {
	params.fitness_sum = 0;
	params.highest_fitness = 0;
	int i;
	for (i = 0; i < params.pop_size; i++) {
		struct individual *ind = &(population[i]);
		ind->fitness = count_ones(ind);
		params.fitness_sum += ind->fitness;
		if (ind->fitness > params.highest_fitness) {
			params.highest_fitness = ind->fitness;
		}
	}
}

void init_population() {
	population = calloc(params.pop_size, sizeof(struct individual));
	population_next = calloc(params.pop_size, sizeof(struct individual));
	int i;
	for (i = 0; i < params.pop_size; i++) {
		struct individual *ind = &(population[i]);
		ind->id = i;
		ind->string = calloc(params.string_size_in_ints, sizeof(int));
		int n;
		for (n = 0; n < params.string_size_in_ints; n++) {
			ind->string[n] = rand();
		}
	}
	recalculate_fitness();
}

static float get_number_between_0_and_1() {
	float num = rand();
	return num / RAND_MAX;
}

static float *selection_weights;

// TODO: only initialise selection weights once per generation
// Note: static array above will be reused so we don't need to call malloc each iteration
static struct individual *select_individual() {
	float weight_sum = 0.0f;
	float prev_prob = 0.0f;
	int n;
	for (n = 0; n < params.pop_size; n++) {
		struct individual *ind = &(population[n]);
		selection_weights[n] = ((float)ind->fitness / (float)params.fitness_sum) + prev_prob;
		weight_sum = weight_sum + selection_weights[n];
		prev_prob = selection_weights[n];
	}
	int i;
	float val = get_number_between_0_and_1() * weight_sum;
	for (i = 0; i < params.pop_size; i++) {
		val = val - selection_weights[i];
		if (val <= 0) {
			return &(population[i]);
		}
	}
	// handle any rounding error by returning last item
	return &(population[params.pop_size - 1]);
}

static void do_selection_stage() {
	int i;
	for (i = 0; i < params.pop_size; i++) {
		struct individual *sel = select_individual();
		population_next[i] = *sel;
	}
	struct individual *temp = population;
	population = population_next;
	population_next = temp;
}

void print_state() {
	printf("===============================================\n");
	printf("Fitness sum: %d\n", params.fitness_sum);
	int i;
	for (i = 0; i < params.pop_size; i++) {
		//printf("Individual %d, fitness: %d, value 0x%08x\n", population[i].id, population[i].fitness, population[i].string[0]);
	}
	printf("===============================================\n");
}

#define UPPER_MASK 0xFFFF0000
#define LOWER_MASK 0x0000FFFF

// TODO: more advanced crossover
void do_crossover_stage() {
	int i, j;
	for (i = 0; i < params.pop_size; i++) {
		int chance = rand() % 100;
		if (chance >= (params.crossover_rate * 100)) {
			// TODO: exclude self from selection ?
			struct individual *ind1 = &(population[i]);
			struct individual *ind2 = select_individual();
			for (j = 0; j < params.string_size_in_ints; j++) {
				int i_bottom = ind1->string[j] & LOWER_MASK;
				int n_top = ind2->string[j] & UPPER_MASK;
				// zero out bottom of i string and top of n string
				ind1->string[j] = ind1->string[j] & UPPER_MASK;
				ind2->string[j] = ind2->string[j] & LOWER_MASK;
				// now put the bottom of i string into the top of n string and vice-verse
				ind1->string[j] = ind1->string[j] | (n_top >> 16);
				ind2->string[j] = ind2->string[j] | (i_bottom << 16);
			}
		}
	}
}

// Selects a bit to flip using XOR.
// If chromosome is made of multiple ints it flips a bit in each.
void do_mutation_stage() {
	int i, j;
	for (i = 0; i < params.pop_size; i++) {
		int chance = rand() % 100;
		if ((params.mutation_rate * 100) >= chance) {
			for (j = 0; j < params.string_size_in_ints; j++) {
				int bit_num = rand() % params.string_size;
				int mask = pow(2, bit_num);
				population[i].string[j] = population[i].string[j] ^ mask;
			}

		}
	}
}

void do_iteration() {
	do_selection_stage();
	do_crossover_stage();
	do_mutation_stage();
	recalculate_fitness();
}

// TODO: read from args
void init() {
	srand(time(NULL));
	params.crossover_rate = 0.6; // 60%
	params.mutation_rate = 0.001; // 0.1%
	params.pop_size = 100;
	params.string_size = 32;
	params.string_size_in_ints = params.string_size / 32;
	selection_weights = calloc(params.pop_size, sizeof(float));
}

int main(void) {
	init();
	init_population();
	printf("%d, %d\n", 0, params.fitness_sum);
	//print_state();
	int i;
	for (i = 1; i < 100; i++) {
		do_iteration();
		printf("%d, %d\n", i, params.fitness_sum);
		//print_state();
	}
	//print_state();
	return 0;
}
