#include "common.h"

static float get_number_between_0_and_1() {
	float num = rand();
	return num / RAND_MAX;
}

// Need to recalculate selection_weights each time as after this function is 
// called chromosomes will change.
// Note: static array above will be reused so we don't need to call malloc each iteration
static int select_player() {
	float weight_sum = 0.0f;
	float prev_prob = 0.0f;
	int n;
	for (n = 0; n < pop_size; n++) {
		selection_weights[n] = ((float)time_saved[n] / (float)total_time_saved) + prev_prob;
		//printf("Assigned weight of %f\n", selection_weights[n]);
		weight_sum = weight_sum + selection_weights[n];
		prev_prob = selection_weights[n];
	}
	int i;
	float val = get_number_between_0_and_1() * weight_sum;
	for (i = 0; i < pop_size; i++) {
		val = val - selection_weights[i];
		if (val <= 0) {
			return i;
		}
	}
	// handle any rounding error by returning last item
	return pop_size - 1;
}


static void do_selection() {
	int i;
	for (i = 0; i < pop_size; i++) {
		int sel = select_player();
		chromosomes_next[i] = chromosomes[sel];
	}
	int *temp = chromosomes;
	chromosomes = chromosomes_next;
	chromosomes_next = temp;
}

// Given a point creates a mask to extract bits after that point
// For example: with point = 2 it should return 4 
static int create_mask(int point) {
	int mask = 0;
	int i;
	for (i = 0; i < point; i++) {
		mask = mask << 1;
		mask = mask + 1;
	}
	return mask;
}

static void do_crossover() {
	int i;
	for (i = 0; i < pop_size; i++) {
		int chance = rand() % 100;
		if (chance >= (crossover_rate * 100)) {
			// select crossover partner (that isn't the same player)
			int n;
			do {
				n = select_player();
			} while (n == i);
			int chromosome_1 = chromosomes[i];
			int chromosome_2 = chromosomes[n];
			// determine the crossover point and get a mask to do that
			// TODO: precompute masks to save time
			int point = rand() % CHROMOSOME_SIZE;
			int mask = create_mask(point);
			// extract everything after the crossover point
			int swap_1 = chromosome_1 & mask;
			int swap_2 = chromosome_2 & mask;
			// zero out everything after the crossover point in the original
			chromosomes[i] = (chromosomes[i] >> point) << point;
			chromosomes[n] = (chromosomes[n] >> point) << point;
			// now insert the swapped part
			chromosomes[i] += swap_2;
			chromosomes[n] += swap_1;
		}
	}
}

// Selects a bit to flip using XOR.
static void do_mutation() {
	int i;
	for (i = 0; i < pop_size; i++) {
		int chance = rand() % 100;
		if ((mutation_rate * 100) >= chance) {
			int bit_num = rand() % CHROMOSOME_SIZE;
			int mask = 1 << bit_num; // 2 ^ bit_num
			chromosomes[i] = chromosomes[i] ^ mask;
		}
	}
}

// Does the selection, crossover and mutation
void do_genetic_part(){
	do_selection();
	do_crossover();
	do_mutation();
}
