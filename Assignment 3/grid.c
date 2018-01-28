#include "common.h"

static void print_local_grid(){
	printf("=======================================\n");
	int i, n;
	for(i = 0; i < LOCAL_NROWS; i++){
		for(n = 0; n < LOCAL_NCOLS; n++){
			//printf("%d, %d\n", i, n);
			printf("%f, ", grid[i][n]);
		}
		printf("\n");
	}
	printf("=======================================\n");
}

void print_all_grids(){
	int i;
	for(i = 0; i < NUM_NODES; i++){
		if(i == RANK){
			print_local_grid();
			sleep(1);
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}
}

static void create_grid(){
	grid = calloc(LOCAL_NROWS, sizeof(double *));
	double *temp = calloc(LOCAL_NCOLS * LOCAL_NROWS, sizeof(double));
	int i;	
	for(i = 0; i < LOCAL_NROWS; i++){
		grid[i] = &(temp[LOCAL_NCOLS * i]);
	}
	prev_grid = calloc(LOCAL_NROWS, sizeof(double *));
	temp = calloc(NCOLS * LOCAL_NROWS, sizeof(double));
	for(i = 0; i < LOCAL_NROWS; i++){
		prev_grid[i] = &(temp[LOCAL_NCOLS * i]);
	}
	int count = 1;
	int n;
	for(i = 0; i < LOCAL_NROWS; i++){
		for(n = 0; n < LOCAL_NCOLS; n++){
			grid[i][n] = count;
			count++;
		}
	}
}

#define LEFT_VALUE 175
#define RIGHT_VALUE 125
#define TOP_VALUE 200
#define BOTTOM_VALUE 150

void init_grid(){
	create_grid();
	int i;
	// Node touches the bottom
	if(LOCAL_Y_COORD == MPI_CART_DIMS[Y_INDEX] - 1){
		int limit;
		int start;
		// Don't overwrite left/right edges
		// TODO: > 2 horizontal divisions
		if(LOCAL_X_COORD == MPI_CART_DIMS[X_INDEX] - 1){
			limit = LOCAL_NCOLS - 1;
			start = 0;
		} else {
			limit = LOCAL_NCOLS;
			start = 1;
		}
		for(i = start; i < limit; i++){
			grid[LOCAL_NROWS-1][i] = BOTTOM_VALUE;
			prev_grid[LOCAL_NROWS-1][i] = BOTTOM_VALUE;
		}
	}
	// Node touches the top
	if(LOCAL_Y_COORD == 0){
		int limit;
		int start;
		// Don't overwrite left/right edges
		// TODO: > 2 horizontal divisions
		if(LOCAL_X_COORD == MPI_CART_DIMS[X_INDEX] - 1){
			limit = LOCAL_NCOLS - 1;
			start = 0;
		} else {
			limit = LOCAL_NCOLS;
			start = 1;
		}
		for(i = start; i < limit; i++){
			grid[0][i] = TOP_VALUE;
			prev_grid[0][i] = TOP_VALUE;
		}
	}
	// Node touches the left side
	if(LOCAL_X_COORD == 0){
		for(i = 0; i < LOCAL_NROWS; i++){
			grid[i][0] = LEFT_VALUE;
			prev_grid[i][0] = LEFT_VALUE;
		}
	}
	// Node touches the right side
	if(LOCAL_X_COORD == MPI_CART_DIMS[X_INDEX] - 1){
		for(i = 0; i < LOCAL_NROWS; i++){
			grid[i][LOCAL_NCOLS-1] = RIGHT_VALUE;
			prev_grid[i][LOCAL_NCOLS-1] = RIGHT_VALUE;
		}
	}
}

