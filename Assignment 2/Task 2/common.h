#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CHROMOSOME_SIZE 20

// Params to be read in from the command line
int pop_size;
int num_generations;
int num_pd_games_per_iter;
float crossover_rate;
float mutation_rate;

// Data for each player
// Use the player's ID to index into here
// NOTE: see report for chromosome structure
int *chromosomes;
int *chromosomes_next;
int *time_saved;
int total_time_saved;
float avg_time_saved;

void reset_time_saved();
void save_total_time_saved();
void do_round_robin();

float *selection_weights;
void do_genetic_part();
