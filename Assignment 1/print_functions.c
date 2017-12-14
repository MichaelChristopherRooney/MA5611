////////////////////////////////////////////////////////////
// Print functions for debugging
////////////////////////////////////////////////////////////

#include "common.h"

extern struct p_data local_data;

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

void print_initial_and_control(){
	printf("A:\n");
	print_matrix(A_MAT, MAT_SIZE, MAT_SIZE);
	printf("B:\n");
	print_matrix(B_MAT, MAT_SIZE, MAT_SIZE);
	printf("Control:\n");
	print_matrix(CONTROL_MAT, MAT_SIZE, MAT_SIZE);
}

void print_initial_send(int dest, int i, int j, int shift, double **a_temp, double **b_temp){
	printf("========================================\n");
	printf("Sending to P %d:\n", dest);
	printf("A[%d][%d] :\n", i, shift);
	print_matrix(a_temp, PART_SIZE, PART_SIZE);
	printf("B[%d][%d]:\n", shift, j);	
	print_matrix(b_temp, PART_SIZE, PART_SIZE);
	printf("i: %d, j: %d\n", i, j);
	printf("========================================\n");
}

void print_initial_receive(){
	printf("========================================\n");
	printf("P %d got i = %d, j = %d\n", local_data.rank, local_data.i, local_data.j);
	printf("A:\n");
	print_matrix(local_data.a_part, PART_SIZE, PART_SIZE);
	printf("B:\n");
	print_matrix(local_data.b_part, PART_SIZE, PART_SIZE);
	printf("========================================\n");
}

void print_partial_results(){
	int i;
	for(i = 0; i < NUM_PARTS; i++) {
		MPI_Barrier(MPI_COMM_WORLD);
		if (i == local_data.rank) {
			printf("Final C for %d:\n", local_data.rank);
			print_matrix(local_data.c_part, PART_SIZE, PART_SIZE);
			fflush(stdout);			
		}
	}	
}
