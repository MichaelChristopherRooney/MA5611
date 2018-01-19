#include "common.h"

static void reset_time_saved(){
	total_time_saved = 0;
	memset(time_saved, 0, pop_size * sizeof(unsigned long long));
	if(rank == 0){
		memset(time_saved_recv_buffer, 0, pop_size * sizeof(unsigned long long));
	}
}

static void init_chromosomes(){
	int i;
	for (i = 0; i < pop_size; i++) {
		if (RAND_MAX <= 0xFFFF) {
			chromosomes[i] = (rand() << 16) + rand();
		} else {
			chromosomes[i] = rand();
		}
	}
}

static void init(char *argv[]) {
	srand(time(NULL));
	pop_size = atoi(argv[1]);
	num_generations = atoi(argv[2]);
	num_pd_games_per_iter = atoi(argv[3]);
	crossover_rate = atof(argv[4]);
	mutation_rate = atof(argv[5]);
	chromosomes = malloc(pop_size * sizeof(int));
	chromosomes_next = malloc(pop_size * sizeof(int));
	time_saved = malloc(pop_size * sizeof(unsigned long long));
	total_time_saved = 0;
	selection_weights = malloc(pop_size * sizeof(float));
	num_worker_nodes = num_nodes - 1;
	work_block_size = pop_size / num_worker_nodes;
	if(rank == 0){
		time_saved_recv_buffer = malloc(pop_size * sizeof(unsigned long long));
		init_chromosomes();	
	}
}

// Sends a broadcast of ALL chromosomes to each rank besides root
static void send_chromosomes_to_slaves(){
	MPI_Bcast(chromosomes, pop_size, MPI_INT, 0, MPI_COMM_WORLD);
}

// Receive chromosome data from master
static void get_chromosomes_from_master(){
	MPI_Bcast(chromosomes, pop_size, MPI_INT, 0, MPI_COMM_WORLD);
}

// Send partial result for time saved back to master using MPI _Reduce.
void send_partial_time_saved_to_master(){
	MPI_Reduce(time_saved, NULL, pop_size, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
}

// Get time saved back from slaves.
// Each slave computes a partial time saved for each chromosome.
// Using MPI_Reduce MPI will sum the partial results for us.
// Note: the time_saved and time_saved_recv_buffer arrays should be zero'd out before this,
// as the master will take part in the sum too, so we need to ensure it isn't actually doing
// anything.
static void receive_partial_time_saved_from_slaves(){
	reset_time_saved();
	MPI_Reduce(time_saved, time_saved_recv_buffer, pop_size, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
	memcpy(time_saved, time_saved_recv_buffer, pop_size * sizeof(unsigned long long));
	get_total_time_saved();
}

int main(int argc, char *argv[]){
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if(argc != 6){
		if(rank == 0){
			printf("Expected usage: ./prog pop_size num_generations num_iterations crossover_rate mutation_rate\n");
		}
		MPI_Finalize();
		return 0;
	}
	MPI_Comm_size(MPI_COMM_WORLD, &num_nodes);
	init(argv);
	int i;
	for (i = 0; i < num_generations; i++) {
		if(rank == 0){
			send_chromosomes_to_slaves();
			receive_partial_time_saved_from_slaves();	
			do_genetic_part();
		} else {
			reset_time_saved();
			get_chromosomes_from_master();
			do_round_robin();
			send_partial_time_saved_to_master();
		}
	}
	MPI_Finalize();
	return 0;
}

