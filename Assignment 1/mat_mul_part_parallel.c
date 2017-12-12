#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define MAT_SIZE 4
#define NUM_ROW_DIVISIONS 2
#define NUM_COL_DIVISIONS 2
static const int NUM_PARTS = NUM_ROW_DIVISIONS * NUM_COL_DIVISIONS;
static const int PART_SIZE = MAT_SIZE / NUM_COL_DIVISIONS;

void print_matrix(double **mat, int m, int n) {
	int i, j;
	for (i = 0; i < m; i++) {
		for (j = 0; j < n; j++) {
			printf("%f\t ", mat[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

double **create_empty_matrix(int m, int n) {
	double **result = malloc(sizeof(double) * m);
	double *mat = malloc(sizeof(double) * m * n);
	int i;
	for (i = 0; i < m; i++) {
		result[i] = &(mat[i * n]);
	}
	return result;
}

double **create_matrix_with_random_values(int m, int n) {
	double **mat = create_empty_matrix(m, n);
	int i, j;
	for (i = 0; i < m; i++) {
		for (j = 0; j < n; j++) {
			mat[i][j] = rand() % 5; // numbers in range 0-4
			mat[i][j] -= 2; // adjust range to -2-+2
		}
	}
	return mat;
}

void extract_part(double **source, double **dest, int start_row, int start_col){
	int i, j;
	for(i = start_row; i < start_row + PART_SIZE; i++){
		for(j = start_col; j < start_col + PART_SIZE; j++){
			dest[i - start_row][j - start_col] = source[i][j];
		}
	}	
}

void send_initial_part(double **a, double **b, double **a_temp, double **b_temp, int i, int j){
	int dest = j + (i*2);
	int shift = (i + j) % ((int) sqrt(NUM_PARTS));
	printf("P[%d][%d] (rank %d) should get:\n", i, j, dest);
	printf("A[%d][%d] :\n", i, shift);
	extract_part(a, a_temp, i*PART_SIZE, shift*PART_SIZE);
	print_matrix(a_temp, PART_SIZE, PART_SIZE);
	printf("B[%d][%d]:\n", shift, j);
	extract_part(b, b_temp, shift*PART_SIZE, j*PART_SIZE);
	print_matrix(b_temp, PART_SIZE, PART_SIZE);
	MPI_Send(*a_temp, PART_SIZE * PART_SIZE, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
	MPI_Send(*b_temp, PART_SIZE * PART_SIZE, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
	sleep(1);
}

// Sends the initial parts to the corresponding destination node.
void send_all_initial_parts(double **a, double **b){
	double **a_temp = create_empty_matrix(PART_SIZE, PART_SIZE);
	double **b_temp = create_empty_matrix(PART_SIZE, PART_SIZE);
	int i, j;
	for(i = 0; i < PART_SIZE; i++){
		for(j = 0; j < PART_SIZE; j++){
			if(i == 0 && j == 0){
				continue;
			}
			send_initial_part(a, b, a_temp, b_temp, i, j);
		}
	}
}

void receive_initial_parts(int rank){
	MPI_Status s;
	double **a_part = malloc(sizeof(double *) * PART_SIZE);
	double **b_part = malloc(sizeof(double *) * PART_SIZE);
	double *a_buf = malloc(sizeof(double) * PART_SIZE * PART_SIZE);
	double *b_buf = malloc(sizeof(double) * PART_SIZE * PART_SIZE);
	MPI_Recv(a_buf, PART_SIZE * PART_SIZE, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &s);
	MPI_Recv(b_buf, PART_SIZE * PART_SIZE, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &s);
	int i;
	for(i = 0; i < PART_SIZE; i++){
		a_part[i] = &(a_buf[i*PART_SIZE]);
		b_part[i] = &(b_buf[i*PART_SIZE]);
	}
	printf("P %d got matrix A:\n", rank);
	print_matrix(a_part, PART_SIZE, PART_SIZE);
	printf("B:\n");
	print_matrix(b_part, PART_SIZE, PART_SIZE);
}

int main(int argc, char *argv[]){
	MPI_Init(&argc, &argv);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//printf("Hello from %d\n", rank);
	double **a_part = create_empty_matrix(PART_SIZE, PART_SIZE);
	double **b_part = create_empty_matrix(PART_SIZE, PART_SIZE);
	if(rank == 0){
		double **a = create_matrix_with_random_values(MAT_SIZE, MAT_SIZE);
		double **b = create_matrix_with_random_values(MAT_SIZE, MAT_SIZE);
		printf("A:\n");
		print_matrix(a, MAT_SIZE, MAT_SIZE);
		printf("B:\n");
		print_matrix(b, MAT_SIZE, MAT_SIZE);
		send_all_initial_parts(a, b);
	} else {
		receive_initial_parts(rank);
	}
	MPI_Finalize();
	return 0;
}
