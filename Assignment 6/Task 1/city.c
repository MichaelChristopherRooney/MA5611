#include <math.h>
#include <stdlib.h>

#include "common.h"

float get_distance_between_cities(struct city *c1, struct city *c2){
	float temp = pow(c1->x - c2->x, 2) + pow(c1->y - c2->y, 2);
	return sqrt(temp);
}

// TODO: seed RNG once done developing
void generate_cities(){
	CITIES = calloc(NUM_CITIES, sizeof(struct city));
	int i;
	for(i = 0; i < NUM_CITIES; i++){
		CITIES[i].id = i + 1;
		CITIES[i].x = (float) drand48();
		CITIES[i].y = (float) drand48();
		//printf("City %d is located at %f, %f\n", i, CITIES[i].x, CITIES[i].y);
	}
}

// TODO:
static float solve_recursive(struct city *cur, int depth, int *visited, float distance){
	return 0.0f;
}

// A wrapper to start the recursive search.
// It's important that the "visited" array is created with calloc.
// Old values in that array will cause issues.
// Uses the first city as a starting point.
float solve_tsp(){
	int *visited = calloc(NUM_CITIES, sizeof(int));
	visited[0] = CITIES[0].id;
	float dist = solve_recursive(&CITIES[0], 1, visited, 0.0f);
	free(visited);
	return dist;
}
