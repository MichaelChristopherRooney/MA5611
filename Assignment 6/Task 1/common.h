int NUM_CITIES;

struct city {
	int id;
	float x;
	float y;
};

struct city *CITIES;

float get_distance_between_cities(struct city *c1, struct city *c2);
void generate_cities();
float solve_tsp();
