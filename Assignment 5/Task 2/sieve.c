#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

int *init_prime_array(const int n){
	int *primes = calloc(n, sizeof(int));
	memset(primes, 1, n*sizeof(int));
	int i;
	for(i = 2; i < n;){
		primes[i] = 1;
		int j;
		// Mark all multiples of i as non-prime
		for(j = i + i; j < n; j = j + i){
			primes[j] = 0;
		}
		i++;
		// Increment i until we find the next number that is still marked prime
		while(i < n && primes[i] == 0){
			i++;		
		}
	}
	return primes;
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
	struct timeval start_time;
	struct timeval end_time;
	long long time_taken;
	gettimeofday(&start_time, NULL);
	int *primes = init_prime_array(n);
	gettimeofday(&end_time, NULL);
	time_taken = (end_time.tv_sec - start_time.tv_sec) * 1000000L + (end_time.tv_usec - start_time.tv_usec);
	printf("Time taken: %lld microseconds.\n", time_taken);
	int i;
	for(i = 0; i < n; i++){
		if(primes[i] == 1){
			//printf("%d is prime\n", i);
		}
	}
	return 0;
}
