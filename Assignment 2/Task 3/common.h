#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mpi.h>

int rank;
int num_nodes;
int num_worker_nodes;
int work_block_size;

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
unsigned long long *time_saved;
unsigned long long *time_saved_recv_buffer;
unsigned long long total_time_saved;

#define CHROMOSOME_SIZE 20

float *selection_weights;
void do_round_robin();
void do_genetic_part();
void get_total_time_saved();
