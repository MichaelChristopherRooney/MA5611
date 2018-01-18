#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mpi.h>

static int rank;
static int num_nodes;
static int num_worker_nodes;
static int work_block_size;

// Params to be read in from the command line
static int pop_size;
static int num_generations;
static int num_pd_games_per_iter;
static float crossover_rate;
static float mutation_rate;

enum game_choice {
	COOPERATE = 0,
	DEFECT = 1,
};

// Data for each player
// Use the player's ID to index into here
// NOTE: see report for chromosome structure
static int *chromosomes;
static int *chromosomes_next;
static int *time_saved;
static int total_time_saved;
static float avg_time_saved;

#define CHROMOSOME_SIZE 20

enum game_choice get_player_choice(int p_id, int game_num, int game_results) {
	if (game_num < 2) {
		int choice;
		if (game_num == 0) {
			choice = chromosomes[p_id] & 0x3; // extract bits 0 and 1
		} else {
			choice = (chromosomes[p_id] & 0xC) >> 2; // extract bits 2 and 3
		}
		if (choice == 2 || choice == 3) {
			return rand() % 2;
		}
		return choice;
	} else {
		return ((chromosomes[p_id] >> 4) >> game_results) & 0x1;
	}
}

// If even num store in bits 0 and 1, else bits 2 and 3
static int save_results(int prev_results, int new_results, int game_num) {
	if (game_num % 2 == 0) {
		prev_results = prev_results & 0x12; // clear bits 0 and 1
		new_results = new_results + prev_results;
	} else {
		prev_results = prev_results & 0x3; // clear bits 2 and 3
		new_results = (new_results << 2) + prev_results;
	}
	return new_results;
}

static void print_choices(enum game_choice p1_c, enum game_choice p2_c) {
	if (p1_c == COOPERATE) {
		printf("Player 1 is cooperating\n");
	} else {
		printf("Player 1 is defecting\n");
	}
	if (p2_c == COOPERATE) {
		printf("Player 2 is cooperating\n");
	} else {
		printf("Player 2 is defecting\n");
	}
}

// TODO: look up a reward matrix ?
static void get_reward(int p_id_1, int p_id_2, enum game_choice p1_c, enum game_choice p2_c) {
	if (p1_c == COOPERATE) {
		if (p2_c == COOPERATE) {
			time_saved[p_id_1] += 3;
			time_saved[p_id_2] += 3;
		} else {
			time_saved[p_id_1] += 0;
			time_saved[p_id_2] += 5;
		}
	} else {
		if (p2_c == COOPERATE) {
			time_saved[p_id_1] += 5;
			time_saved[p_id_2] += 0;
		} else {
			time_saved[p_id_1] += 1;
			time_saved[p_id_2] += 1;
		}
	}
}


static int play_round(int p_id_1, int p_id_2, int game_num, int prev_results) {
	enum game_choice p1_c = get_player_choice(p_id_1, game_num, prev_results);
	enum game_choice p2_c = get_player_choice(p_id_2, game_num, prev_results);
	//print_choices(p1_c, p2_c);
	get_reward(p_id_1, p_id_2, p1_c, p2_c);
	int results = (p2_c << 1) + p1_c;
	return results;
}

static void reset_time_saved() {
	memset(time_saved, 0, pop_size * sizeof(int));
	total_time_saved = 0;
}

static void print_time_saved() {
	int i;
	for (i = 0; i < pop_size; i++) {
		printf("Player %d's time saved is: %d\n", i, time_saved[i]);
	}
}

static void save_total_time_saved() {
	int i;
	for (i = 0; i < pop_size; i++) {
		total_time_saved += time_saved[i];
	}
	avg_time_saved = (float)total_time_saved / (float)pop_size;
	printf("Total time saved: %d\n", total_time_saved);
	//printf("Average time saved: %f\n", avg_time_saved);
}

// Every player should play every other player
/*
static void do_round_robin() {
	int block_start = (rank - 1) * work_block_size; // rank - 1 as root is not doing this work
	int block_end = (rank * work_block_size) - 1;
	printf("%d is handling block starting at %d and ending at %d\n", rank, block_start, block_end);
	int i, n, j;
	for(i = block_start; i <= block_end; i++){
		for(n = 0; n < pop_size; n++){
			if(i == n){
				continue;
			}
			printf("Rank %d: %d is playing %d\n", rank, i, n);
			int game_results = 0;
			for (j = 0; j < num_pd_games_per_iter; j++) {
				int temp_results = play_round(i, n, j, game_results);
				game_results = save_results(game_results, temp_results, j);
			}
		}
	}
}
*/

// Every player should play every other player
static void do_round_robin() {
	int i, n, j;
	for (i = 0; i < pop_size; i++) {
		n = (rank - 1) + i + 1;
		for (n; n < pop_size; n = n + num_worker_nodes) {
			int game_results = 0;
			for (j = 0; j < num_pd_games_per_iter; j++) {
				int temp_results = play_round(i, n, j, game_results);
				game_results = save_results(game_results, temp_results, j);
			}
		}
	}
}

static float get_number_between_0_and_1() {
	float num = rand();
	return num / RAND_MAX;
}


static float *selection_weights;

// TODO: only initialise selection weights once per generation
// TODO: double check that this is working
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
	int i, j;
	for (i = 0; i < pop_size; i++) {
		int chance = rand() % 100;
		if ((mutation_rate * 100) >= chance) {
			int bit_num = rand() % CHROMOSOME_SIZE;
			int mask = 1 << bit_num; // 2 ^ bit_num
			chromosomes[i] = chromosomes[i] ^ mask;
			int a = 0;
		}
	}
}

// TODO: read from args
static void init() {
	srand(time(NULL));
	pop_size = 10;
	num_generations = 1;
	num_pd_games_per_iter = 5;
	crossover_rate = 0.6f; // 60%
	mutation_rate = 0.001f; // 0.1%
	chromosomes = malloc(pop_size * sizeof(int));
	chromosomes_next = malloc(pop_size * sizeof(int));
	time_saved = calloc(pop_size, sizeof(int));
	total_time_saved = 0;
	selection_weights = malloc(pop_size * sizeof(float));
	num_worker_nodes = num_nodes - 1;
	work_block_size = pop_size / num_worker_nodes;
	if(rank == 0){
		int i;
		for (i = 0; i < pop_size; i++) {
			if (RAND_MAX <= 0xFFFF) {
				//chromosomes[i] = (rand() << 16) + rand();
			} else {
				//chromosomes[i] = rand();make
			}
			chromosomes[i] = 0;
		}
	
	}
}

// Sends a broadcast of ALL chromosomes to each rank besides root
static void send_data_to_slaves(){
	MPI_Bcast(chromosomes, pop_size, MPI_INT, 0, MPI_COMM_WORLD);
}

static void get_data_from_master(){
	MPI_Status s;

	MPI_Bcast(chromosomes, pop_size, MPI_INT, 0, MPI_COMM_WORLD);
}

int main(int argc, char *argv[]){
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_nodes);
	init();
	int i;
	for (i = 0; i < 1; i++) {
		if(rank == 0){
			reset_time_saved();
			send_data_to_slaves();
			//save_total_time_saved();
			//do_selection();
			//do_crossover();
			//do_mutation();
		} else {
			get_data_from_master();
			sleep(3);
			do_round_robin();
			save_total_time_saved();
		}
		
	}
	MPI_Finalize();
	return 0;
}

