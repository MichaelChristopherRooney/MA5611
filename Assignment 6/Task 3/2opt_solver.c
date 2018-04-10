#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "common.h"	
/*
       1. take route[0] to route[i-1] and add them in order to new_route
       2. take route[i] to route[k] and add them in reverse order to new_route
       3. take route[k+1] to end and add them in order to new_route
*/
static void do_2opt_swap(int *cur_route, int *new_route, int i, int k){
	int n;
	for(n = 0; n < i; n++){
		new_route[n] = cur_route[n];
	}
	for(n = i; n <= k; n++){
		int index = i + (k - n);
		new_route[n] = cur_route[index];
	}
	for(n = k + 1; n < NUM_CITIES; n++){
		new_route[n] = cur_route[n];
	}

}

static float calculate_route_distance(int *route){
	float dist = 0.0f;
	int i;
	for(i = 0; i < NUM_CITIES - 1; i++){
		int id_1 = route[i];
		int id_2 = route[i+1];
		dist += CITIES[id_1-1].distances[id_2-1];
	}
	return dist;
}

// Existing is a route that should comes from a greedy nearest neighbour solution.
// Dist will contain the distanced of the new calulcated route.
// Returns the route as an int array.
int *solve_tsp_2opt(int *existing, float existing_dist, float *dist){
	int *new_route = calloc(NUM_CITIES, sizeof(int));
	int *best_route = calloc(NUM_CITIES, sizeof(int));
	memcpy(best_route, existing, NUM_CITIES * sizeof(int));
	float best_dist = existing_dist;
	int improvement = 1;
	while(improvement){
		improvement = 0;
		int i, k;
		for(i = 1; i < NUM_CITIES - 1; i++){
			for(k = i + 1; k < NUM_CITIES; k++){
				do_2opt_swap(best_route, new_route, i, k);
				float new_dist = calculate_route_distance(new_route);
				if(new_dist < best_dist){
					improvement = 1;
					best_dist = new_dist;
					int *temp = new_route;
					new_route = best_route;
					best_route = temp;
				}
			}
		}
	}
	*dist = best_dist;
	return best_route;
}
