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

void generate_cities(){
	srand48(time(NULL));
	CITIES = calloc(NUM_CITIES, sizeof(struct city));
	int i, j;
	for(i = 0; i < NUM_CITIES; i++){
		CITIES[i].id = i + 1;
		CITIES[i].x = (float) drand48() * 100;
		CITIES[i].y = (float) drand48() * 100;
		CITIES[i].distances = calloc(NUM_CITIES, sizeof(float));
		printf("City with id %d is located at %f, %f\n", i+1, CITIES[i].x, CITIES[i].y);
	}
	// Precompute the distances between each city
	for(i = 0; i < NUM_CITIES - 1; i++){
		for(j = 1; j < NUM_CITIES; j++){
			float distance = get_distance_between_cities(&CITIES[i], &CITIES[j]);
			CITIES[i].distances[j] = distance;
			CITIES[j].distances[i] = distance;
		}
	}
}

static int *visited;
static int *shortest_route;
static float shortest_distance = 0.0;

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
