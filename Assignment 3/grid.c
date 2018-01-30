#include "common.h"

// Note: doesn't print extra space used for receiving from other nodes
static void print_local_grid(){
	printf("=======================================\n");
	int i, n;
	for(i = 1; i < LOCAL_NROWS - 1; i++){
		for(n = 1; n < LOCAL_NCOLS - 1; n++){
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
	// For debug to see where things are being transfered
	int count = 11;
	int n;
	for(i = 1; i < LOCAL_NROWS - 1; i++){
		for(n = 1; n < LOCAL_NCOLS - 1; n++){
			//grid[i][n] = count;
			count++;
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

