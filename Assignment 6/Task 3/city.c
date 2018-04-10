#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "common.h"

static int *visited_greedy;
static int *shortest_route_greedy;
static float shortest_distance_greedy = 0.0;

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
	// Format is "DIMENSION : 123" so we can just skip to the number part
	NUM_CITIES = atoi(line + 12);
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

static int already_visited_greedy(struct city *cur, int depth){
	int i;
	for(i = 0; i < depth; i++){
		if(visited_greedy[i] == cur->id){
			return 1;
		}
	}
	return 0;
}

// TODO:
static void solve_recursive_greedy(struct city *cur, int depth, float distance){
	visited_greedy[depth-1] = cur->id;
	if(depth == NUM_CITIES){
		//printf("Found path with distance: %f\n", distance);
		if(distance < shortest_distance_greedy || shortest_distance_greedy == 0.0){
			memcpy(shortest_route_greedy, visited_greedy, NUM_CITIES * sizeof(int));
			shortest_distance_greedy = distance;
		}
		return;
	}
	struct city *next_city = NULL;
	float closest_distance = 0.0f;
	int i;
	for(i = 0; i < NUM_CITIES; i++){
		if(cur->id == i + 1){
			continue;
		}
		if(already_visited_greedy(&CITIES[i], depth) == 0){
			if(cur->distances[i] < closest_distance || closest_distance == 0.0f){
				next_city = &CITIES[i];
				closest_distance = cur->distances[i];
			}
		}
	}
	//printf("Closest city to %d is %d with distance %f\n", cur->id, next_city->id, closest_distance);
	float next_distance = distance + cur->distances[next_city->id - 1];
	solve_recursive_greedy(next_city, depth + 1, next_distance);
}

// A wrapper to start the recursive search.
// Uses a greedy algortihm to get a quick upper bound
// It's important that the "visited" array is created with calloc.
// Old values in that array will cause issues.
int *solve_tsp_greedy(float *dist){
	visited_greedy = calloc(NUM_CITIES, sizeof(int));
	shortest_route_greedy = calloc(NUM_CITIES, sizeof(int));
	int i;
	for(i = 0; i < NUM_CITIES; i++){
		solve_recursive_greedy(&CITIES[i], 1, 0.0f);
	}
	free(visited_greedy);
	*dist = shortest_distance_greedy;
	return shortest_route_greedy;
}
