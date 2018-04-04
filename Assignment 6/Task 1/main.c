#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "common.h"

static void parse_args(int argc, char *argv[]){
	NUM_CITIES = 0; // to check if it is set below
	int c;
	while((c = getopt(argc, argv, "n")) != -1){
		switch(c){
		case 'n':
			NUM_CITIES = atoi(argv[optind]);
		}
	}
	if(NUM_CITIES <= 0){
		printf("ERROR: please provide a positive non-zero value for n\n");
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
	generate_cities();
	struct timeval start, end;
	gettimeofday(&start, NULL);
	float dist;
	int *path = solve_tsp(&dist);
	gettimeofday(&end, NULL);
	long long time_taken = (end.tv_sec - start.tv_sec) * 1000000L + (end.tv_usec - start.tv_usec);
	print_results(dist, path, time_taken);
	free(CITIES);
	free(path);
	return 0;
}
