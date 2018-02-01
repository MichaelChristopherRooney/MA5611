#include "common.h"

// For debugging
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

// For debugging
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

// For debugging
void print_recv_grid(){
	printf("=======================================\n");
	int i, n;
	int x = 0;
	for(i = x; i < LOCAL_NROWS - x; i++){
		for(n = x; n < LOCAL_NCOLS - x; n++){
			//printf("%d, %d\n", i, n);
			printf("%f, ", recv_grid[i][n]);
		}
		printf("\n");
	}
	printf("=======================================\n");
}


// Should only be called by rank 0 in the cart topology
void print_final_grid(){
	int i, n;
	int x = 0;
	for(i = x; i < NROWS - x; i++){
		for(n = x; n < NCOLS - x; n++){
			printf("%f, ", final_grid[i][n]);
		}
		printf("\n");
	}
}
static double **create_grid(int nrow, int ncol){
	double **temp1 = calloc(nrow, sizeof(double *));
	double *temp2 = calloc(ncol * nrow, sizeof(double));
	int i;	
	for(i = 0; i < nrow; i++){
		temp1[i] = &(temp2[ncol * i]);
	}
	return temp1;
}

// TODO: move some of this code into other functions
static void create_grids(){
	grid = create_grid(LOCAL_NROWS, LOCAL_NCOLS);
	prev_grid = create_grid(LOCAL_NROWS, LOCAL_NCOLS);
	if(CART_RANK == 0){
		final_grid = create_grid(NROWS, NCOLS);
		recv_grid = create_grid(LOCAL_NROWS, LOCAL_NCOLS);
	}
}

// TODO change these back when done debugging
#define LEFT_VALUE 75
#define RIGHT_VALUE 25
#define TOP_VALUE 100
#define BOTTOM_VALUE 50

void init_grid(){
	create_grids();
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

void free_grid(double **grid){
	free(grid[0]);
	free(grid);
}


