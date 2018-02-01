#include "common.h"

static void print_local_grid(){
	printf("=======================================\n");
	printf("Rank %d at x = %d and y = %d\n", RANK, LOCAL_X_COORD, LOCAL_Y_COORD);
	int i, n;
	int x = 1; // set to 1 to avoid printing recv buffers
	for(i = x; i < LOCAL_NROWS - x; i++){
		for(n = x; n < LOCAL_NCOLS - x; n++){
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

// TODO: move some of this code into other functions
static void create_grid(){
	grid = calloc(LOCAL_NROWS, sizeof(double *));
	double *temp = calloc(LOCAL_NCOLS * LOCAL_NROWS, sizeof(double));
	int i;	
	for(i = 0; i < LOCAL_NROWS; i++){
		grid[i] = &(temp[LOCAL_NCOLS * i]);
	}
	prev_grid = calloc(LOCAL_NROWS, sizeof(double *));
	temp = calloc(LOCAL_NCOLS * LOCAL_NROWS, sizeof(double));
	for(i = 0; i < LOCAL_NROWS; i++){
		prev_grid[i] = &(temp[LOCAL_NCOLS * i]);
	}
	// For debug to see where things are being transfered
	int count = 10 * RANK;
	int n;
	for(i = 1; i < LOCAL_NROWS - 1; i++){
		for(n = 1; n < LOCAL_NCOLS - 1; n++){
			//grid[i][n] = count;
			count++;
		}
	}
	if(RANK == 0){
		final_grid = calloc(NROWS, sizeof(double *));
		temp = calloc(NCOLS * NROWS, sizeof(double));
		for(i = 0; i < NROWS; i++){
			final_grid[i] = &(temp[NCOLS * i]);
		}
		recv_grid = calloc(LOCAL_NROWS, sizeof(double *));
		temp = calloc(LOCAL_NCOLS * LOCAL_NROWS, sizeof(double));
		for(i = 0; i < LOCAL_NROWS; i++){
			recv_grid[i] = &(temp[LOCAL_NCOLS * i]);
		}
	}
}

// TODO change these back when done debugging
#define LEFT_VALUE 75
#define RIGHT_VALUE 25
#define TOP_VALUE 100
#define BOTTOM_VALUE 50

void init_grid(){
	create_grid();
	int i;
	// Node touches the bottom
	if(LOCAL_Y_COORD == MPI_CART_DIMS[Y_INDEX] - 1){
		for(i = 1; i < LOCAL_NCOLS - 1; i++){
			grid[LOCAL_NROWS-2][i] = BOTTOM_VALUE;
			prev_grid[LOCAL_NROWS-2][i] = BOTTOM_VALUE;
		}
	}
	// Node touches the top
	if(LOCAL_Y_COORD == 0){
		for(i = 1; i < LOCAL_NCOLS - 1; i++){
			grid[1][i] = TOP_VALUE;
			prev_grid[1][i] = TOP_VALUE;
		}
	}
	// Node touches the left side
	if(LOCAL_X_COORD == 0){
		for(i = 1; i < LOCAL_NROWS - 1; i++){
			grid[i][1] = LEFT_VALUE;
			prev_grid[i][1] = LEFT_VALUE;
		}
	}
	// Node touches the right side
	if(LOCAL_X_COORD == MPI_CART_DIMS[X_INDEX] - 1){
		for(i = 1; i < LOCAL_NROWS - 1; i++){
			grid[i][LOCAL_NCOLS-2] = RIGHT_VALUE;
			prev_grid[i][LOCAL_NCOLS-2] = RIGHT_VALUE;
		}
	}
}

