#include <stdio.h>

// Params to be read in from the command line
// Mutation and crossover rate should be in range [0, 100]
static int pop_size;
static int num_generations;
static int num_pd_iterations;
static int crossover_rate;
static int mutation_rate;

enum strategy {
	ALWAYS_DEFECT = 0,
	ALWAYS_COOPERATE = 1
};

enum game_choice {
	COOPERATE = 0,
	DEFECT = 1
};

enum game_state {
	CC = 0,
	CD = 1, // p1 defects
	DC = 2, // p2 defects
	DD = 3
};

// TODO: look up a reward matrix ?
void get_reward(enum game_choice p1_c, enum game_choice p2_c, int *p1_time, int *p2_time){
	if(p1_c == COOPERATE){
		if(p2_c == COOPERATE){
			*p1_time = 3;
			*p2_time = 3;
		} else {
			*p1_time = 0;
			*p2_time = 5;
		}
	} else {
		if(p2_c == COOPERATE){
			*p1_time = 5;
			*p2_time = 0;
		} else {
			*p1_time = 1;
			*p2_time = 1;
		}
	}
}

void print_results(enum game_choice p1_c, enum game_choice p2_c, int p1_time, int p2_time){
	if(p1_c == COOPERATE){
		printf("P1 cooperated\n");
	} else {
		printf("P1 defected\n");
	}
	if(p2_c == COOPERATE){
		printf("P2 cooperated\n");
	} else {
		printf("P2 defected\n");
	}
	printf("P1 saved %d year(s)\n", p1_time);
	printf("P2 saved %d year(s)\n", p2_time);
}

enum game_choice get_choice(enum strategy s){
	switch(s){
	case ALWAYS_DEFECT:
		return DEFECT;
	case ALWAYS_COOPERATE:
		return COOPERATE;
	}
}

void play_round(enum strategy p1_s, enum strategy p2_s){
	enum game_choice p1_c = get_choice(p1_s);
	enum game_choice p2_c = get_choice(p2_s);
	int p1_time, p2_time;
	get_reward(p1_c, p2_c, &p1_time, &p2_time);
	print_results(p1_c, p2_c, p1_time, p2_time);
}

int main(void){
	play_round(ALWAYS_DEFECT, ALWAYS_COOPERATE);
	return 0;
}

