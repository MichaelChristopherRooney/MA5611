#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "common.h"

static int *visited_greedy;
static int *shortest_route_greedy;
static float shortest_distance_greedy = 0.0;

static int already_visited_greedy(struct city *cur, int depth){
	int i;
	for(i = 0; i < depth; i++){
		if(visited_greedy[i] == cur->id){
			return 1;
		}
	}
	return 0;
}

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
// Dist will contain the distanced of the new calulcated route.
// Returns the route as an int array.
int *solve_tsp_greedy(float *dist){
	visited_greedy = calloc(NUM_CITIES, sizeof(int));
	shortest_route_greedy = calloc(NUM_CITIES, sizeof(int));
	if(FIXED_START_CITY != -1){
		solve_recursive_greedy(&CITIES[FIXED_START_CITY - 1], 1, 0.0f);
	} else {
		int i;
		for(i = 0; i < NUM_CITIES; i++){
			solve_recursive_greedy(&CITIES[i], 1, 0.0f);
		}
	}
	free(visited_greedy);
	*dist = shortest_distance_greedy;
	return shortest_route_greedy;
}
