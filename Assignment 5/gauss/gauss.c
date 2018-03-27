#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <omp.h>

#define MAX_VALUE 5
#define MIN_VALUE 1

static float **copy_system(float **source, const int n){
	float **copy = calloc(1, sizeof(float*) * n);
	float *temp = calloc(1, sizeof(float) * n * (n+1));
	int i;
	for(i = 0; i < n; i++){
		copy[i] = &(temp[i*(n+1)]);
	}
	int j;
	for(i = 0; i < n; i++){
		for(j = 0; j < n + 1; j++){
			copy[i][j] = source[i][j];
		}
	}
	return copy;
}

static void check_systems_equal(float **s1, float **s2, const int n){
	float epsilon = 1E-6;
	int i, j;
	for(i = 0; i < n; i++){
		for(j = 0; j < n + 1; j++){
			if((s1[i][j] - s2[i][j]) > epsilon){
				printf("Systems are not equal!\n");
				exit(1);
			}
		}
	}
	printf("Systems are equal\n");
}

static void fill_with_values(float **system, const int n){
	int i, j;
	for(i = 0; i < n; i++){
		for(j = 0; j < n + 1; j++){
			system[i][j] = rand() % (MAX_VALUE + 1 - MIN_VALUE) + MIN_VALUE;
		}
	}
}

// Note: n+1 is used here a bit.
// If we have n variables then there must be n+1 colums.
static float **create_random_system(const int n){
	float **system = calloc(1, sizeof(float*) * n);
	float *temp = calloc(1, sizeof(float) * n * (n+1));
	int i;
	for(i = 0; i < n; i++){
		system[i] = &(temp[i*(n+1)]);
	}
	fill_with_values(system, n);
	return system;
}

static void print_system(float **system, const int n){
	int i, j;
	for(i = 0; i < n; i++){
		for(j = 0; j < n + 1; j++){
			printf("%f, ", system[i][j]);
		}
		printf("\n");
	}
}

static int parse_args(int argc, char *argv[]){
	if(argc != 3){
		printf("Please provide a value for n\n");
		exit(1);
	}
	int c;
	while((c = getopt(argc, argv, "n")) != -1){
		switch(c){
		case 'n':
			return atoi(argv[optind]);
		}
	}
	printf("Please provide a value for n\n");
	exit(1);
}

void solve_serial(float **system, const int n);
void solve_openmp(float **system, const int n);

static void time_serial(float **system, const int n){
	struct timeval start_time;
	struct timeval end_time;
	long long time_taken;
	gettimeofday(&start_time, NULL);
	solve_serial(system, n);
	gettimeofday(&end_time, NULL);
	time_taken = (end_time.tv_sec - start_time.tv_sec) * 1000000L + (end_time.tv_usec - start_time.tv_usec);
	printf("Serial time taken: %lld microseconds.\n", time_taken);
}

static void time_openmp(float **system, const int n){
	struct timeval start_time;
	struct timeval end_time;
	long long time_taken;
	gettimeofday(&start_time, NULL);
	solve_openmp(system, n);
	gettimeofday(&end_time, NULL);
	time_taken = (end_time.tv_sec - start_time.tv_sec) * 1000000L + (end_time.tv_usec - start_time.tv_usec);
	printf("OpenMP time taken: %lld microseconds.\n", time_taken);
}

int main(int argc, char *argv[]){
	int n = parse_args(argc, argv);
	float **serial_system = create_random_system(n);
	float **openmp_system = copy_system(serial_system, n);
	time_serial(serial_system, n);
	time_openmp(openmp_system, n);
	check_systems_equal(serial_system, openmp_system, n);
	return 0;
}
