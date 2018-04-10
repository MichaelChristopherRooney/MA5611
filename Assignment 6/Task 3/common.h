int NUM_CITIES;
char *INPUT_FILE;

struct city {
	int id;
	float x;
	float y;
	float *distances;
};

struct city *CITIES;

void init_cities();
int *solve_tsp_greedy(float *dist);


