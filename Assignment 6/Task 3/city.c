#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "common.h"

static float get_distance_between_cities(struct city *c1, struct city *c2){
	float temp = pow(c1->x - c2->x, 2) + pow(c1->y - c2->y, 2);
	return sqrt(temp);
}

static void precompute_distances(){
	int i, j;
	for(i = 0; i < NUM_CITIES - 1; i++){
		for(j = 1; j < NUM_CITIES; j++){
			float distance = get_distance_between_cities(&CITIES[i], &CITIES[j]);
			CITIES[i].distances[j] = distance;
			CITIES[j].distances[i] = distance;
		}
	}
}

// Create a city struct given a node coord line from the input file
static void load_city_from_string(int i, char *line){
	char *id_str = strtok(line, " ");
	char *x_str = strtok(NULL, " ");
	char *y_str = strtok(NULL, " ");
	if(id_str == NULL || x_str == NULL || y_str == NULL){
		printf("ERROR: Bad node format\n");
		exit(1);
	}
	CITIES[i].id = atoi(id_str);
	CITIES[i].x = atof(x_str);
	CITIES[i].y = atof(y_str);
	CITIES[i].distances = calloc(NUM_CITIES, sizeof(float));
	printf("City with id %d is located at %f, %f\n", CITIES[i].id, CITIES[i].x, CITIES[i].y);
}

// Gets the DIMENSION value from the headers and also locates the node coord section.
// WHen this returns fp should point to the first node coord entry.
static void parse_file_headers(FILE *fp){
	char *line = "";
	size_t size = 0;
	size_t read = 0;
	// First search the header for the number of cities
	while(strstr(line, "DIMENSION") == NULL){
		read = getline(&line, &size, fp);
		if(read == -1){
			printf("ERROR: Failed to find DIMENSION in TSP header.\n");
			exit(1);
		}
	}
	// Now we have the line with the dimension on it
	// Format is either "DIMENSION : 123" or "DIMENSION: 123" (note gaps around ':')
	// so we look for ": " and jump over that.
	char *dim_str = strtok(line, ": ");
	dim_str = strtok(NULL, ": ");
	NUM_CITIES = atoi(dim_str);
	if(FIXED_START_CITY != -1 && FIXED_START_CITY > NUM_CITIES + 1){
		printf("ERROR: fixed starting city does not exist\n");
		exit(1);
	}
	printf("CITIES: %d\n", NUM_CITIES);
	CITIES = calloc(NUM_CITIES, sizeof(struct city));
	// Now skip over the remaining headers until we find the node coord section
	while(strcmp(line, "NODE_COORD_SECTION\n") != 0 && read != -1){
		read = getline(&line, &size, fp);
		if(read == -1){
			printf("ERROR: Failed to find NODE COORD SECTION in TSP header.\n");
			exit(1);
		}
	}
}

void init_cities(){
	FILE *fp = fopen(INPUT_FILE, "rb");
	if(fp == NULL){
		printf("ERROR: cannot open file to read from.\n");
		exit(1);
	}
	parse_file_headers(fp);
	char *line = NULL;
	size_t size = 0;
	size_t read = 0;
	// Load each city's data from the node coord section
	int i;
	for(i = 0; i < NUM_CITIES; i++){
		read = getline(&line, &size, fp);
		if(read == -1){
			printf("ERROR: EOF before all cities have been read.\n");
			exit(1);
		}
		load_city_from_string(i, line);
	}
	precompute_distances();
	free(line);
}

