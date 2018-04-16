int NUM_CITIES;
int FIXED_START_CITY; // set to -1 if no fixed starting city
char *INPUT_FILE;

struct city {
	int id;
	float x;
	float y;
	float *distances;
};

struct city *CITIES;

void init_cities();
int *solve_tsp(float *dist);


