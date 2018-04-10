#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "common.h"

static int *visited;
static int *shortest_route;
static float shortest_distance = 0.0;

static float get_distance_between_cities(struct city *c1, struct city *c2){
	float temp = pow(c1->x - c2->x, 2) + pow(c1->y - c2->y, 2);
	return sqrt(temp);
}

void init_cities(){
	FILE *fp = fopen(INPUT_FILE, "rb");
	if(fp == NULL){
		printf("ERROR: cannot open file to read from.\n");
		exit(1);
	}
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
	// Load each city's data from the node coord section
	int i;
	for(i = 0; i < NUM_CITIES; i++){
		read = getline(&line, &size, fp);
		if(read == -1){
			printf("ERROR: EOF before all cities have been read.\n");
			exit(1);
		}
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
	// Now precompute the distances between each city
	int j;
	for(i = 0; i < NUM_CITIES - 1; i++){
		for(j = 1; j < NUM_CITIES; j++){
			float distance = get_distance_between_cities(&CITIES[i], &CITIES[j]);
			CITIES[i].distances[j] = distance;
			CITIES[j].distances[i] = distance;
		}
	}
}

static int already_visited(struct city *cur, int depth){
	int i;
	for(i = 0; i < depth; i++){
		if(visited[i] == cur->id){
			return 1;
		}
	}
	return 0;
}

// TODO:
static void solve_recursive(struct city *cur, int depth, float distance){
	visited[depth-1] = cur->id;
	if(depth == NUM_CITIES){
		//printf("Found path with distance: %f\n", distance);
		if(distance < shortest_distance || shortest_distance == 0.0){
			memcpy(shortest_route, visited, NUM_CITIES * sizeof(int));
			shortest_distance = distance;
		}
		return;
	}
	int i;
	for(i = 0; i < NUM_CITIES; i++){
		if(cur->id == i + 1){
			continue;
		}
		if(already_visited(&CITIES[i], depth) == 0){
			struct city *next_city = &CITIES[i];
			//printf("From city id %d going to %d\n", cur->id, next_city->id);
			float next_distance = distance + cur->distances[i];
			solve_recursive(next_city, depth + 1, next_distance);
		}
	}
}

// A wrapper to start the recursive search.
// It's important that the "visited" array is created with calloc.
// Old values in that array will cause issues.
// Uses the first city as a starting point.
int *solve_tsp(float *dist){
	
	visited = calloc(NUM_CITIES, sizeof(int));
	shortest_route = calloc(NUM_CITIES, sizeof(int));
	int i;
	for(i = 0; i < NUM_CITIES; i++){
		solve_recursive(&CITIES[i], 1, 0.0f);
	}
	free(visited);
	*dist = shortest_distance;
	return shortest_route;
}
