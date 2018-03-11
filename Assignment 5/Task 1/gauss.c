#include <stdio.h>
#include <stdlib.h>

#define MAX_VALUE 5
#define MIN_VALUE 1

static void fill_with_values(float **system, const int n){
	int i, j;
	for(i = 0; i < n; i++){
		for(j = 0; j < n + 1; j++){
			system[i][j] = rand() % (MAX_VALUE + 1 - MIN_VALUE) + MIN_VALUE;
		}
	}
}

// Note: n+1 is used here a bit.
// If we have n variables then there must be n+1 colums.
static float **create_random_system(const int n){
	float **system = calloc(1, sizeof(float*) * n);
	float *temp = calloc(1, sizeof(float) * n * (n+1));
	int i;
	for(i = 0; i < n; i++){
		system[i] = &(temp[i*(n+1)]);
	}
	//fill_with_values(system, n);
	// Just an example for testing
	system[0][0] = 2.0;
	system[0][1] = 1.0;
	system[0][2] = -1.0;
	system[0][3] = 8.0;
	system[1][0] = -3.0;
	system[1][1] = -1.0;
	system[1][2] = 2.0;
	system[1][3] = -11.0;
	system[2][0] = -2.0;
	system[2][1] = 1.0;
	system[2][2] = 2.0;
	system[2][3] = -3.0;
	return system;
}

static void print_system(float **system, const int n){
	int i, j;
	for(i = 0; i < n; i++){
		for(j = 0; j < n + 1; j++){
			printf("%f, ", system[i][j]);
		}
		printf("\n");
	}
}

// row1 = (row2*ratio) + row1
static void subtract_rows_with_ratio(float **system, float ratio, int row1, int row2, int n){
	for(int col = 0; col < n + 1; col++){ // note n + 1
		system[row1][col] = (system[row2][col] * ratio) + system[row1][col];
	}
}

static void do_gaussian_elimination(float **system, const int n){
	int row, col;
	for(col = 0; col < n; col++){
		for(row = col + 1; row < n; row++){
			float ratio = (system[row][col] / system[col][col]) * -1.0;
			subtract_rows_with_ratio(system, ratio, row, col, n);
		}
	}
}

static void do_back_substituion(float **system, const int n){
	int row, col;
	for(col = n - 1; col > 0; col--){
		for(row = col - 1; row >= 0; row--){
			float ratio = (system[row][col] / system[col][col]) * -1.0;
			subtract_rows_with_ratio(system, ratio, row, col, n);
		}
	}
	// Normalise - everything on the left should be 1.0
	for(row = 0; row < n; row++){
		if(system[row][row] != 1.0f){
			float temp = system[row][row];
			system[row][row] /= temp;
			system[row][n] /= temp;
		}
	}
}

// TODO: args
// TODO: allow values to be zero
int main(void){
	int n = 3;
	float **system = create_random_system(n);
	//print_system(system, n);
	do_gaussian_elimination(system, n);
	do_back_substituion(system, n);
	print_system(system, n);
	return 0;
}
