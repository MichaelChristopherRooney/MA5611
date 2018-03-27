#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>
#include <omp.h>

char *sieve_openmp_synchronised(const int n){
	char *primes = calloc(n, sizeof(int));
	memset(primes, 1, (n*sizeof(int)) + 1);
	primes[0] = 0;
	primes[1] = 0;
	int i;
	// first set all multiplies of two to be non-prime
	#pragma omp parallel for
	for(i = 4; i < n; i = i + 2){
		primes[i] = 0;
	}
	i = 3;
	int lim = sqrt(n);
	int lowest_start = 0;
	#pragma omp parallel shared(i)
	{
		int num_threads = omp_get_num_threads();
		int thread_id = omp_get_thread_num();
		int start = i + (i * thread_id);
		int tid;
		do {
			int k;
			for(k = start + start; k <= n; k = k + start){
				primes[k] = 0;
			}
			#pragma omp barrier
			for(tid = 0; tid < num_threads; tid++){
				if(tid == thread_id){
					for(i = i + 1; i <= n; i++){
						if(primes[i] == 1){
							start = i;
							//printf("Thread %d, start %d\n", thread_id, start);
							break;
						}
					}
					if(tid == 0){
						lowest_start = start;
					}
				}
			#pragma omp barrier
			}
		}while(lowest_start <= lim);
		
	}
	return primes;
}

char *sieve_openmp_dynamic(const int n){
	char *primes = calloc(n, sizeof(int));
	memset(primes, 1, (n*sizeof(int)) + 1);
	int lim = sqrt(n);
	int i;
	#pragma omp parallel for private(i) schedule(dynamic)
	for (i = 2; i <= lim; i++){
		if (primes[i]){
			int j;
			for (j = i * i; j <= n; j += i){
				primes[j] = 0;
			}
		}
	}
	primes[0] = 0;
	primes[1] = 0;
	return primes;
}

char *sieve_openmp_static(const int n){
	char *primes = calloc(n, sizeof(int));
	memset(primes, 1, (n*sizeof(int)) + 1);
	int lim = sqrt(n);
	int i;
	#pragma omp parallel for private(i) schedule(static)
	for (i = 2; i <= lim; i++){
		if (primes[i]){
			int j;
			for (j = i * i; j <= n; j += i){
				primes[j] = 0;
			}
		}
	}
	primes[0] = 0;
	primes[1] = 0;
	return primes;
}

char *sieve_serial(const int n){
	char *primes = calloc(n, sizeof(int));
	memset(primes, 1, (n*sizeof(int)) + 1);
	int lim = sqrt(n);
	int i;
	for (i = 2; i <= lim; i++){
		if (primes[i]){
			int j;
			for (j = i * i; j <= n; j += i){
				primes[j] = 0;
			}
		}
	}
	primes[0] = 0;
	primes[1] = 0;
	return primes;
}

#define NUM_FUNCS 4

char *names[NUM_FUNCS] = { "Serial", "OpenMP static", "OpenMP dynamic", "OpenMP synchronised" };
char *(*sieves[NUM_FUNCS]) (const int n) = { 
	&sieve_serial, &sieve_openmp_static, &sieve_openmp_dynamic, &sieve_openmp_synchronised
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

#define CHECK_RESULTS 1

static void time_sieves(const int n){
	int i;
	for(i = 0; i < NUM_FUNCS; i++){
		struct timeval start_time;
		struct timeval end_time;
		long long time_taken;
		gettimeofday(&start_time, NULL);
		char *r = sieves[i](n);
#if CHECK_RESULTS
		results[i] = r;
#else
		free(r);
#endif
		gettimeofday(&end_time, NULL);
		time_taken = (end_time.tv_sec - start_time.tv_sec) * 1000000L + (end_time.tv_usec - start_time.tv_usec);
		printf("%s took:\t%lld microseconds.\n", names[i], time_taken);
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
#if CHECK_RESULTS
	check_results(n);
#endif
	return 0;
} 
