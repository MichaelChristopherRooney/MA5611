#include <stdlib.h>
#include <stdio.h>
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

void mat_mul_part(double **a, double **b, double **c, int num_rows, int num_cols) {
	const int part_size = 2;
	int i;
	double **temp1 = create_empty_matrix(part_size, part_size);
	double **temp2 = create_empty_matrix(part_size, part_size);
	for (i = 0; i < part_size; i++) {
		int j;
		for (j = 0; j < part_size; j++) {
			int k;
			// TODO: sum the related parts - ensure old memory is cleared
			for (k = 0; k < part_size; k++) {
				mat_mul_single_part(a, b, temp1, part_size, i*part_size, k*part_size, k*part_size, j*part_size);
				print_matrix(temp1, part_size, part_size);

			}

		}
	}
}

int main(int argc, char *argv[]) {
	int n = 4;
	double **a = create_matrix_with_random_values(n, n);
	double **b = create_matrix_with_random_values(n, n);
	double **c = create_empty_matrix(n, n);
	mat_mul_part(a, b, c, n, n);
	//printf("a\n");
	//print_matrix(a, n, n);
	//printf("b\n");
	//print_matrix(b, n, n);
	//printf("c\n");
	//print_matrix(c, n, n);
	return 0;
}
