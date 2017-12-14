////////////////////////////////////////////////////////////
// Functions for creating and working with matrices
////////////////////////////////////////////////////////////

#include "common.h"

// Frees matrices allocated using the single large malloc method
void free_mat(double **mat){
	free(*mat);
	free(mat);
}

void set_matrix_from_buf(double **mat, double *buf, int m, int n){
	int i;
	for (i = 0; i < m; i++) {
		mat[i] = &(buf[i * n]);
	}
}

// Given a "flat" array this will set up a double** array to be the matrix
double **create_matrix_from_buf(double *buf, int m, int n){
	double **result = malloc(sizeof(double) * m);
	int i;
	for (i = 0; i < m; i++) {
		result[i] = &(buf[i * n]);
	}
	return result;
}

double **create_empty_matrix(int m, int n) {
	double **result = malloc(sizeof(double) * m);
	double *mat = calloc(sizeof(double),  m * n); // needs calloc - got weird results using malloc
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

void mat_mul_serial(double **a, double **b, double **c, int l, int m, int n) {
	int i, j, k;
	for (i = 0; i < l; i++) {
		for (j = 0; j < n; j++) {
			double sum = 0.0;
			for (k = 0; k < m; k++) {
				sum += a[i][k] * b[k][j];
			}
			c[i][j] = sum;
		}
	}
}

// Note that this uses c[i][j] += sum;
// This will called multiple times with the same c and we need to sum the results.
void mat_mul_part(double **a, double **b, double **c, int l, int m, int n) {
	int i, j, k;
	for (i = 0; i < l; i++) {
		for (j = 0; j < n; j++) {
			double sum = 0.0;
			for (k = 0; k < m; k++) {
				sum += (a[i][k] * b[k][j]);
			}
			c[i][j] += sum;
		}
	}
}

void extract_part(double **source, double **dest, int start_row, int start_col){
	int i, j;
	for(i = start_row; i < start_row + PART_SIZE; i++){
		for(j = start_col; j < start_col + PART_SIZE; j++){
			dest[i - start_row][j - start_col] = source[i][j];
		}
	}	
}

// Inserts a partial result into the final result matrix
void insert_part(double **source, double **dest, int start_row, int start_col){
	int i, j;
	for(i = 0; i < PART_SIZE; i++){
		for(j = 0; j < PART_SIZE; j++){
			dest[i + start_row][j + start_col] = source[i][j];
		}
	}	
}

void create_initial_matrices(){	
	A_MAT = create_matrix_with_random_values(MAT_SIZE, MAT_SIZE);
	B_MAT = create_matrix_with_random_values(MAT_SIZE, MAT_SIZE);	
	CONTROL_MAT = create_empty_matrix(MAT_SIZE, MAT_SIZE);
	RESULT_MAT = create_empty_matrix(MAT_SIZE, MAT_SIZE);
	mat_mul_serial(A_MAT, B_MAT, CONTROL_MAT, MAT_SIZE, MAT_SIZE, MAT_SIZE);
	print_initial_and_control();
}

void check_result_matches_control(){
	int i, j;
	for(i = 0; i < MAT_SIZE; i++){
		for(j = 0; j < MAT_SIZE; j++){
			if(CONTROL_MAT[i][j] != RESULT_MAT[i][j]){
				printf("ERROR: serial and parallel results differ!\n");
				return;
			}
		}
	}
	printf("SUCCESS: serial and parallel results match!\n");
}
