#include "common.h"

static void init(char *argv[]) {
	srand(time(NULL));
	pop_size = atoi(argv[1]);
	num_generations = atoi(argv[2]);
	num_pd_games_per_iter = atoi(argv[3]);
	crossover_rate = atof(argv[4]);
	mutation_rate = atof(argv[5]);
	chromosomes = malloc(pop_size * sizeof(int));
	chromosomes_next = malloc(pop_size * sizeof(int));
	time_saved = calloc(pop_size, sizeof(unsigned long long));
	total_time_saved = 0;
	selection_weights = malloc(pop_size * sizeof(float));
	int i;
	for (i = 0; i < pop_size; i++) {
		if (RAND_MAX <= 0xFFFF) {
			chromosomes[i] = (rand() << 16) + rand();
		} else {
			chromosomes[i] = rand();
		}
	}
}

int main(int argc, char *argv[]) {
	if(argc != 6){
		printf("Expected usage: ./prog pop_size num_generations num_iterations crossover_rate mutation_rate\n");
		return 1;
	}
	init(argv);
	int i;
	for (i = 0; i < num_generations; i++) {
		reset_time_saved();
		do_round_robin();
		save_total_time_saved();
		do_genetic_part();
	}
	return 0;
}

