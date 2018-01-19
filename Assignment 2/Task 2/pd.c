#include "common.h"

enum game_choice {
	COOPERATE = 0,
	DEFECT = 1,
};

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
	get_reward(p_id_1, p_id_2, p1_c, p2_c);
	int results = (p2_c << 1) + p1_c;
	return results;
}

void reset_time_saved() {
	memset(time_saved, 0, pop_size * sizeof(int));
	total_time_saved = 0;
}

void save_total_time_saved() {
	int i;
	for (i = 0; i < pop_size; i++) {
		total_time_saved += time_saved[i];
	}
	avg_time_saved = (float)total_time_saved / (float)pop_size;
	printf("Total time saved: %d\n", total_time_saved);
	//printf("Average time saved: %f\n", avg_time_saved);
}

// Every player should play every other player
void do_round_robin() {
	int i, n, j;
	for (i = 0; i < pop_size - 1; i++) {
		for (n = i + 1; n < pop_size; n++) {
			int game_results = 0;
			for (j = 0; j < num_pd_games_per_iter; j++) {
				int temp_results = play_round(i, n, j, game_results);
				game_results = save_results(game_results, temp_results, j);
			}
		}
	}
}
