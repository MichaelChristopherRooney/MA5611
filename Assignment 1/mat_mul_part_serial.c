#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

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

// Handles multiplying parts of two matrices.
// The offsets are used to index into different parts of the matrices.
void mat_mul_single_part(double **a, double **b, double **c, int part_size, int a_row_offset, int a_col_offset, int b_row_offset, int b_col_offset) {
	int i, j, k;
	for (i = 0; i < part_size; i++) {
		for (j = 0; j < part_size; j++) {
			double sum = 0.0;
			for (k = 0; k < part_size; k++) {
				double temp1 = a[i + a_row_offset][k + a_col_offset];
				double temp2 = b[k + b_row_offset][j + b_col_offset];
				sum += temp1 * temp2;
			}
			c[i][j] = sum;
		}
	}
}

void add_mat(double **dest, double **source, int num_rows, int num_cols) {
	int i, j;
	for (i = 0; i < num_rows; i++) {
		for (j = 0; j < num_cols; j++) {
			dest[i][j] += source[i][j];
		}
	}
}

void copy_part_to_result(double **part, double **result, int num_rows, int num_cols, int row_offset, int col_offset) {
	int i, j;
	for (i = 0; i < num_rows; i++) {
		for (j = 0; j < num_cols; j++) {
			result[i + row_offset][j + col_offset] = part[i][j];
		}
	}
}

void mat_mul_part(double **a, double **b, double **c, int num_rows, int num_cols) {
	const int part_size = 2;
	int i, j, k;
	double **temp1 = create_empty_matrix(part_size, part_size);
	double **temp2 = create_empty_matrix(part_size, part_size);
	for (i = 0; i < part_size; i++) {
		for (j = 0; j < part_size; j++) {
			// Ensure the memory does not contain old values.
			memset(*temp2, 0, part_size * part_size * sizeof(double));
			for (k = 0; k < part_size; k++) {
				mat_mul_single_part(a, b, temp1, part_size, i*part_size, k*part_size, k*part_size, j*part_size);
				add_mat(temp2, temp1, part_size, part_size);
			}
			//print_matrix(temp2, part_size, part_size);
			// Write the partial result to the result matrix.
			copy_part_to_result(temp2, c, part_size, part_size, i * part_size, j * part_size);
		}
	}
}

// matrix a is l x m
// matrix b is m x n
// matrix c is l x n
void mat_mul(double **a, double **b, double **c, int l, int m, int n) {
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

int main(int argc, char *argv[]) {
	int n = 4;
	double **a = create_matrix_with_random_values(n, n);
	double **b = create_matrix_with_random_values(n, n);
	double **part = create_empty_matrix(n, n);
	double **control = create_empty_matrix(n, n);
	mat_mul_part(a, b, part, n, n);
	mat_mul(a, b, control, n, n, n); // control for comparison
	//printf("a\n");
	//print_matrix(a, n, n);
	//printf("b\n");
	//print_matrix(b, n, n);
	printf("Control matrix:\n");
	print_matrix(control, n, n);
	printf("Using part:\n");
	print_matrix(part, n, n);
	return 0;
}
