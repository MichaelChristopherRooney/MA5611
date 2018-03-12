#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>
#include <omp.h>

/* Something like this ?

i = 2
pragma omp parallel
{
	do {
		start = thread_id ...
		end = thread_id ...
		pragma barrier
		if(thread_id == 0)
			find next prime
		pragma barrier
	}(while i < n);
}
*/

char *sieve_with_opt_and_openmp(const int n){
	char *primes = calloc(n, sizeof(int));
	memset(primes, 1, (n*sizeof(int)) + 1);
	int lim = sqrt(n);
	int i;
	omp_set_num_threads(4);
	#pragma omp parallel for schedule(dynamic)
	for (i = 2; i <= lim; i++){
		if (primes[i]){
			for (int j = i * i; j <= n; j += i){
				primes[j] = 0;
			}
		}
	}
	primes[0] = 0;
	primes[1] = 0;
	return primes;
}

char *sieve_with_lim_and_square_opt(const int n){
	char *primes = calloc(n, sizeof(int));
	memset(primes, 1, (n*sizeof(int)) + 1);
	int lim = sqrt(n);
	int i;
	for (i = 2; i <= lim; i++){
		if (primes[i]){
			for (int j = i * i; j <= n; j += i){
				primes[j] = 0;
			}
		}
	}
	primes[0] = 0;
	primes[1] = 0;
	return primes;
}

char *sieve_with_lim_opt(const int n){
	char *primes = calloc(n, sizeof(int));
	memset(primes, 1, (n*sizeof(int)) + 1);
	int lim = sqrt(n);
	int i;
	for (i = 2; i <= lim; i++){
		if (primes[i]){
			for (int j = i + i; j <= n; j += i){
				primes[j] = 0;
			}
		}
	}
	primes[0] = 0;
	primes[1] = 0;
	return primes;
}

char *sieve_naive(const int n){
	char *primes = calloc(n, sizeof(int));
	memset(primes, 1, (n*sizeof(int)) + 1);
	int i;
	for (i = 2; i <= n; i++){
		if (primes[i]){
			for (int j = i + i; j <= n; j += i){
				primes[j] = 0;
			}
		}
	}
	primes[0] = 0;
	primes[1] = 0;
	return primes;
}

#define NUM_FUNCS 4

char *names[NUM_FUNCS] = { "naive", "sqrt opt", "sqrt and sqr opt" };
char *(*sieves[NUM_FUNCS]) (const int n) = { 
	&sieve_naive, &sieve_with_lim_opt, &sieve_with_lim_and_square_opt, &sieve_with_opt_and_openmp
};
char *results[NUM_FUNCS];

static void check_results(const int n){
	int i, k;
	for(i = 0; i < n; i++){
		char r = results[0][i];
		for(k = 1; k < NUM_FUNCS; k++){
			if(results[k][i] != r){
				printf("Error: results do not match\n");
				exit(1);
			}
		}
	}
}

static void time_sieves(const int n){
	int i;
	for(i = 0; i < NUM_FUNCS; i++){
		struct timeval start_time;
		struct timeval end_time;
		long long time_taken;
		gettimeofday(&start_time, NULL);
		results[i] = sieves[i](n);
		gettimeofday(&end_time, NULL);
		time_taken = (end_time.tv_sec - start_time.tv_sec) * 1000000L + (end_time.tv_usec - start_time.tv_usec);
		printf("Time taken: %lld microseconds.\n", time_taken);
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

int main(int argc, char *argv[]){
	int n = parse_args(argc, argv);
	time_sieves(n);
	check_results(n);
	return 0;
} 
