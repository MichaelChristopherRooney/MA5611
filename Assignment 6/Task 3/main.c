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

// Global since the 2opt part needs them
int *greedy_route;
float greedy_dist;

static void do_greedy_part(){
	struct timeval start, end;
	gettimeofday(&start, NULL);
	greedy_route = solve_tsp_greedy(&greedy_dist);
	gettimeofday(&end, NULL);
	long long time_taken_greedy = (end.tv_sec - start.tv_sec) * 1000000L + (end.tv_usec - start.tv_usec);
	printf("==================================\n");
	printf("GREEDY NEAREST NEIGHBOUR RESULTS:\n");
	printf("==================================\n");
	print_results(greedy_dist, greedy_route, time_taken_greedy);
}

static void do_2opt_part(){
	struct timeval start, end;
	gettimeofday(&start, NULL);
	float dist_2opt;
	int *path_2opt = solve_tsp_2opt(greedy_route, greedy_dist, &dist_2opt);
	gettimeofday(&end, NULL);
	long long time_taken_2opt = (end.tv_sec - start.tv_sec) * 1000000L + (end.tv_usec - start.tv_usec);
	printf("==================================\n");
	printf("2OPT RESULTS:\n");
	printf("==================================\n");
	print_results(dist_2opt, path_2opt, time_taken_2opt);
}

int main(int argc, char *argv[]){
	parse_args(argc, argv);
	init_cities();
	do_greedy_part();
	do_2opt_part();
	free(CITIES);
	free(greedy_route);
	return 0;
}
