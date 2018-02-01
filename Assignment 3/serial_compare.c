#include "common.h"

static double **serial_grid;
static double **prev_serial_grid;

static void create_serial_grid(){
	serial_grid = calloc(NROWS, sizeof(double *));
	double *temp = calloc(NCOLS * NROWS, sizeof(double));
	int i;	
	for(i = 0; i < NROWS; i++){
		serial_grid[i] = &(temp[NCOLS * i]);
	}
	prev_serial_grid = calloc(NROWS, sizeof(double *));
	temp = calloc(NCOLS * NROWS, sizeof(double));
	for(i = 0; i < NROWS; i++){
		prev_serial_grid[i] = &(temp[NCOLS * i]);
	}
}

static void do_serial_iteration(){
	int i, n;
	for(i = 1; i < NROWS - 1; i++){
		for(n = 1; n < NCOLS - 1; n++){
			prev_serial_grid[i][n] = (serial_grid[i-1][n] + serial_grid[i+1][n] + serial_grid[i][n-1] + serial_grid[i][n+1]) / 4.0;
		}
	}
	double **temp = serial_grid;
	serial_grid = prev_serial_grid;
	prev_serial_grid = temp;
}

#define LEFT_VALUE 75.0
#define RIGHT_VALUE 25.0
#define TOP_VALUE 100.0
#define BOTTOM_VALUE 50.0

static void init_serial_grid(){
	create_serial_grid();
	int i;
	// Left and right
	for(i = 0; i < NROWS; i++){
		serial_grid[i][0] = LEFT_VALUE;
		serial_grid[i][NCOLS-1] = RIGHT_VALUE;
		prev_serial_grid[i][0] = LEFT_VALUE;
		prev_serial_grid[i][NCOLS-1] = RIGHT_VALUE;
	}
	// Top
	for(i = 1; i < NCOLS - 1; i++){
		serial_grid[0][i] = TOP_VALUE;
		prev_serial_grid[0][i] = TOP_VALUE;
	}
	// Bottom 
	for(i = 1; i < NCOLS - 1; i++){
		serial_grid[NROWS-1][i] = BOTTOM_VALUE;
		prev_serial_grid[NROWS-1][i] = BOTTOM_VALUE;
	}
}

void compare(){
	init_serial_grid();
	int i;
	for(i = 0; i < NUM_ITER; i++){
		do_serial_iteration();
	}
	int n;
	for(i = 0; i < NROWS; i++){
		for(n = 0; n < NCOLS; n++){
			if(final_grid[i][n] != serial_grid[i][n]){
				printf("At row %d col %d:\t parallel got %f, serial got %f\n", i, n, final_grid[i][n], serial_grid[i][n]);
			}
		}
	}
	
}

