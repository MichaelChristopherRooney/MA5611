#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Params to be read in from the command line
// Mutation and crossover rate should be in range [0, 100]
static int pop_size;
static int num_generations;
static int num_pd_games_per_iter;
static int crossover_rate;
static int mutation_rate;

enum game_state {
	CC = 0,
	CD = 1, // p1 defects
	DC = 2, // p2 defects
	DD = 3
};

enum game_choice {
	COOPERATE = 0,
	DEFECT = 1,
};

// NOTE: see report for chromosome structure
struct player_data {
	int chromosome;
	int time_saved;
};

enum game_choice get_player_choice(struct player_data *p, int game_num, int game_results){
	if(game_num < 2){
		int choice;
		if(game_num == 0){
			choice = p->chromosome & 0x3; // extract bits 0 and 1
		} else {
			choice = p->chromosome & 0x12; // extract bits 2 and 3
		}
		if(choice == 2 || choice == 3){
			return rand() % 2;
		}
		return choice;
	} else {
		return ((p->chromosome >> 4) >> game_results) & 0x1;
	}
}

// If even num store in bits 0 and 1, else bits 2 and 3
int save_results(int prev_results, int new_results, int game_num){
	if(game_num % 2 == 0){
		prev_results = prev_results & 0x12; // clear bits 0 and 1
		new_results = new_results + prev_results;
	} else {
		prev_results = prev_results & 0x3; // clear bits 2 and 3
		new_results = (new_results << 2) + prev_results;
	}
	return new_results;
}

void print_choices(enum game_choice p1_c, enum game_choice p2_c){
	if(p1_c == COOPERATE){
		printf("Player 1 is cooperating\n");
	} else {
		printf("Player 1 is defecting\n");
	}
	if(p2_c == COOPERATE){
		printf("Player 2 is cooperating\n");
	} else {
		printf("Player 2 is defecting\n");
	}
}

// TODO: look up a reward matrix ?
void get_reward(struct player_data *p1, struct player_data *p2, enum game_choice p1_c, enum game_choice p2_c){
	if(p1_c == COOPERATE){
		if(p2_c == COOPERATE){
			p1->time_saved += 3;
			p2->time_saved += 3;
		} else {
			p1->time_saved += 0;
			p2->time_saved += 5;
		}
	} else {
		if(p2_c == COOPERATE){
			p1->time_saved += 5;
			p2->time_saved += 0;
		} else {
			p1->time_saved += 1;
			p2->time_saved += 1;
		}
	}
}

int play_round(struct player_data *p1, struct player_data *p2, int game_num, int prev_results){
	enum game_choice p1_c = get_player_choice(p1, game_num, prev_results);
	enum game_choice p2_c = get_player_choice(p2, game_num, prev_results);
	print_choices(p1_c, p2_c);
	get_reward(p1, p2, p1_c, p2_c);
	int results = (p2_c << 1) + p1_c;
	return results;
}

void run_iterations(){
	struct player_data *p1 = calloc(1, sizeof(struct player_data));
	struct player_data *p2 = calloc(1, sizeof(struct player_data));
	p1->chromosome = rand();
	p2->chromosome = rand();
	int game_state = 0;
	int game_results = 0;
	int i;
	for(i = 0; i < num_pd_games_per_iter; i++){
		printf("======================================\n");
		printf("Game number %d\n", i);
		int temp_results = play_round(p1, p2, i, game_results);
		game_results = save_results(game_results, temp_results, i);
		printf("Last two results are: %d\n", game_results);
		printf("======================================\n");
	}
	printf("Player 1 time saved: %d\n", p1->time_saved);
	printf("Player 2 time saved: %d\n", p2->time_saved);

}

int main(void){
	srand(time(NULL));
	pop_size = 2;
	num_generations = 10;
	num_pd_games_per_iter = 5;
	crossover_rate = 60;
	mutation_rate = 10;
	run_iterations();
}

