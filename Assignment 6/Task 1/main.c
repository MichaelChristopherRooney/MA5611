#include <stdio.h>
#include <stdlib.h>
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

int main(int argc, char *argv[]){
	parse_args(argc, argv);
	generate_cities();
	free(CITIES);
	return 0;
}
