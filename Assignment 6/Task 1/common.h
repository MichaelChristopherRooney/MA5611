int NUM_CITIES;

struct city {
	int id;
	float x;
	float y;
	float *distances;
};

struct city *CITIES;

void generate_cities();
int *solve_tsp(float *dist);
