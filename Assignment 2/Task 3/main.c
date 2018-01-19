#include "common.h"

// TODO: read from args
static void init(char *argv[]) {
	srand(time(NULL));
	pop_size = atoi(argv[1]);
	num_generations = atoi(argv[2]);
	num_pd_games_per_iter = atoi(argv[3]);
	crossover_rate = atof(argv[4]);
	mutation_rate = atof(argv[5]);
	chromosomes = malloc(pop_size * sizeof(int));
	chromosomes_next = malloc(pop_size * sizeof(int));
	time_saved = calloc(pop_size, sizeof(int));
	total_time_saved = 0;
	selection_weights = malloc(pop_size * sizeof(float));
	num_worker_nodes = num_nodes - 1;
	work_block_size = pop_size / num_worker_nodes;
	if(rank == 0){
		time_saved_recv_buffer = malloc(pop_size * sizeof(int));
		int i;
		for (i = 0; i < pop_size; i++) {
			if (RAND_MAX <= 0xFFFF) {
				chromosomes[i] = (rand() << 16) + rand();
			} else {
				chromosomes[i] = rand();
			}
			//chromosomes[i] = 0;
		}
	
	}
}

// Sends a broadcast of ALL chromosomes to each rank besides root
static void send_chromosomes_to_slaves(){
	MPI_Bcast(chromosomes, pop_size, MPI_INT, 0, MPI_COMM_WORLD);
}

// Receive chromosome data from master
static void get_chromosomes_from_master(){
	MPI_Status s;
	MPI_Bcast(chromosomes, pop_size, MPI_INT, 0, MPI_COMM_WORLD);
}

// Send time saved back to master
void send_data_to_master(){
	//printf("Rank %d sending data to root\n", rank);
	MPI_Send(time_saved, pop_size, MPI_INT, 0, 0, MPI_COMM_WORLD);
}



// Get time saved back from slaves.
// Each slave computes a partial time saved for each chromosome.
// The master needs to sum the results here.
// The master first resets the current time saved for each chromosome, and the total time saved.
// Note: reset_time_saved should be called before this so the buffer starts with all zeros.
static void receive_time_saved_from_slaves(){
	reset_time_saved();
	int i;
	for(i = 0; i < num_worker_nodes; i++){
		//printf("Root waiting to get data from %d\n", i + 1);
		MPI_Status s;
		MPI_Recv(time_saved_recv_buffer, pop_size, MPI_INT, i + 1, MPI_ANY_TAG, MPI_COMM_WORLD, &s);
		int n;
		for(n = 0; n < pop_size; n++){
			time_saved[n] = time_saved[n] + time_saved_recv_buffer[n];
		}
		//printf("Root got data from %d\n", i + 1);
	}
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
			receive_time_saved_from_slaves();
			do_genetic_part();
		} else {
			reset_time_saved();
			get_chromosomes_from_master();
			do_round_robin();
			send_data_to_master();
		}
	}
	MPI_Finalize();
	return 0;
}

