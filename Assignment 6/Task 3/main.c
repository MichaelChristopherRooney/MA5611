#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "common.h"

// Default null/0 values are set first
static void parse_args(int argc, char *argv[]){
	NUM_CITIES = 0;
	INPUT_FILE = NULL;
	int c;
	while((c = getopt(argc, argv, "f")) != -1){
		switch(c){
		case 'f':
			INPUT_FILE = argv[optind];
			break;
		}
	}
	if(INPUT_FILE == NULL){
		printf("ERROR: please provide an input filename\n");
		exit(1);
	}
}

static void print_results(float dist, int *path, long long time_taken){
	printf("Finding shortest path took %lld microseconds\n", time_taken);
	printf("Shortest distance is %f\n", dist);
	printf("Shortest path is: ");
	int i;
	for(i = 0; i < NUM_CITIES; i++){
		printf("%d, ", path[i]);
	}
	printf("\n");
}

int main(int argc, char *argv[]){
	parse_args(argc, argv);
	init_cities();
	struct timeval start, end;
	gettimeofday(&start, NULL);
	float dist_greedy;
	int *path_greedy = solve_tsp_greedy(&dist_greedy);
	gettimeofday(&end, NULL);
	long long time_taken_greedy = (end.tv_sec - start.tv_sec) * 1000000L + (end.tv_usec - start.tv_usec);
	printf("=== RESULTS FOR GREEDY NEAREST NEIGHBOUR ALGORITHM ===\n");	
	print_results(dist_greedy, path_greedy, time_taken_greedy);
	printf("===================\n");
	free(CITIES);
	free(path_greedy);
	return 0;
}
