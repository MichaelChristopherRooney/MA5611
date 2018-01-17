#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

static void reset_time_saved(){
	memset(time_saved, 0, pop_size * sizeof(int));
	total_time_saved = 0;
}

static void print_time_saved(){
	int i;
	for(i = 0; i < pop_size; i++){
		printf("Player %d's time saved is: %d\n", i, time_saved[i]);
	}
}

static void save_total_time_saved(){
	int i;
	for(i = 0; i < pop_size; i++){
		total_time_saved += time_saved[i];
	}
}

// Every player should play every other player
static void do_round_robin() {
	int i, n, j;
	for(i = 0; i < pop_size - 1; i++){
		for(n = i + 1; n < pop_size; n++){
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
		selection_weights[n] = ((float) time_saved[n] / (float) total_time_saved) + prev_prob;
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


static void do_selection(){
	int i;
	for (i = 0; i < pop_size; i++) {
		int sel = select_player();
		printf("Selected %d\n", sel);
		chromosomes_next[i] = chromosomes[sel];
	}
	int *temp = chromosomes;
	chromosomes = chromosomes_next;
	chromosomes_next = temp;
}

static void do_crossover(){
	
}

// TODO: read from args
static void init(){
	srand(time(NULL));
	pop_size = 10;
	num_generations = 10;
	num_pd_games_per_iter = 5;
	crossover_rate = 0.6; // 60%
	mutation_rate = 0.001; // 0.1%
	chromosomes = malloc(pop_size * sizeof(int));
	chromosomes_next = malloc(pop_size * sizeof(int));
	time_saved = calloc(pop_size, sizeof(int));
	total_time_saved = 0;
	selection_weights = malloc(pop_size * sizeof(float));
	int i;
	for(i = 0; i < pop_size; i++){
		chromosomes[i] = rand();
	}
}

int main(void) {
	init();
	int i;
	for(i = 0; i < 1; i++){
		reset_time_saved();
		do_round_robin();
		save_total_time_saved();
		// TODO: selection, crossover and mutation
		do_selection();
		do_crossover();
		print_time_saved();
	}
	return 0;
}

