#include <stdlib.h>
#include <stdio.h>

static const int NCOLS = 32;
static const int NROWS = 32;

static double **grid;
static double **prev_grid;

static void print_grid(){
	printf("=======================================\n");
	int i, n;
	for(i = 0; i < NROWS; i++){
		for(n = 0; n < NCOLS; n++){
			printf("%f, ", grid[i][n]);
		}
		printf("\n");
	}
	printf("=======================================\n");
}

static void create_grid(){
	grid = calloc(NROWS, sizeof(double *));
	double *temp = calloc(NCOLS * NROWS, sizeof(double));
	int i;	
	for(i = 0; i < NROWS; i++){
		grid[i] = &(temp[NCOLS * i]);
	}
	prev_grid = calloc(NROWS, sizeof(double *));
	temp = calloc(NCOLS * NROWS, sizeof(double));
	for(i = 0; i < NROWS; i++){
		prev_grid[i] = &(temp[NCOLS * i]);
	}
}

static void do_iteration(){
	int i, n;
	for(i = 1; i < NROWS - 1; i++){
		for(n = 1; n < NCOLS - 1; n++){
			prev_grid[i][n] = (grid[i-1][n] + grid[i+1][n] + grid[i][n-1] + grid[i][n+1]) / 4.0;
		}
	}
	double **temp = grid;
	grid = prev_grid;
	prev_grid = temp;
}

#define LEFT_VALUE 75
#define RIGHT_VALUE 25
#define TOP_VALUE 100
#define BOTTOM_VALUE 50

static void init_grid(){
	create_grid();
	int i;
	// Left and right
	for(i = 0; i < NROWS; i++){
		grid[i][0] = LEFT_VALUE;
		grid[i][NCOLS-1] = RIGHT_VALUE;
		prev_grid[i][0] = LEFT_VALUE;
		prev_grid[i][NCOLS-1] = RIGHT_VALUE;
	}
	// Top
	for(i = 1; i < NCOLS - 1; i++){
		grid[0][i] = TOP_VALUE;
		prev_grid[0][i] = TOP_VALUE;
	}
	// Bottom 
	for(i = 1; i < NCOLS - 1; i++){
		grid[NROWS-1][i] = BOTTOM_VALUE;
		prev_grid[NROWS-1][i] = BOTTOM_VALUE;
	}
}

int main(void){
	init_grid();
	//print_grid();
	int i;
	for(i = 0; i < 100; i++){
		do_iteration();
	}
	print_grid();
	return 0;
}
