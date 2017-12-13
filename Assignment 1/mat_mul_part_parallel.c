#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

#define MAT_SIZE 4
#define NUM_ROW_DIVISIONS 2
#define NUM_COL_DIVISIONS 2
static const int NUM_PARTS = NUM_ROW_DIVISIONS * NUM_COL_DIVISIONS;
static const int PART_SIZE = MAT_SIZE / NUM_COL_DIVISIONS;

// Data local to each process
// Collected into a struct to avoid passing many arguments
struct p_data {
	int rank;
	int i;
	int j;
	double **a_part;
	double **b_part;
	double *a_part_buf;
	double *b_part_buf;
	double *a_recv_buf;
	double *b_recv_buf;
	double **c_part;
	// The processes that will received this process's A and B parts
	int a_dest;
	int b_dest;
};

struct p_data data;
struct p_data *my_data = &data;


void set_matrix_from_buf(double **mat, double *buf, int m, int n){
	int i;
	for (i = 0; i < m; i++) {
		mat[i] = &(buf[i * n]);
	}
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
				sum += a[i][k] * b[k][j];
			}
			c[i][j] += sum; 
		}
	}
}

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

void send_initial_part(double **a, double **b, double **a_temp, double **b_temp, int i, int j){
	int dest = j + (i*PART_SIZE);
	int shift = (i + j) % ((int) sqrt(NUM_PARTS));
	//int indices[2] = { i, j };
	extract_part(a, a_temp, i*PART_SIZE, shift*PART_SIZE);
	extract_part(b, b_temp, shift*PART_SIZE, j*PART_SIZE);
	//print_initial_send(dest, i, j, shift, a_temp, b_temp);
	//MPI_Send(indices, 2, MPI_INT, dest, 0, MPI_COMM_WORLD);
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
	free(*a_temp); //TODO: proper free
	free(a_temp);
	free(*b_temp);
	free(b_temp);
}

void print_initial_receive(){
	if(my_data->rank != 1){
		return;
	}
	printf("========================================\n");
	printf("P %d got i = %d, j = %d\n", my_data->rank, my_data->i, my_data->j);
	printf("A:\n");
	print_matrix(my_data->a_part, PART_SIZE, PART_SIZE);
	printf("B:\n");
	print_matrix(my_data->b_part, PART_SIZE, PART_SIZE);
	printf("========================================\n");
}

void receive_initial_parts(){
	MPI_Status s;
	MPI_Recv(my_data->a_recv_buf, PART_SIZE * PART_SIZE, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &s);
	MPI_Recv(my_data->b_recv_buf, PART_SIZE * PART_SIZE, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &s);
	memcpy(my_data->a_part_buf, my_data->a_recv_buf, PART_SIZE * PART_SIZE * sizeof(double));
	memcpy(my_data->b_part_buf, my_data->b_recv_buf, PART_SIZE * PART_SIZE * sizeof(double));
	set_matrix_from_buf(my_data->a_part, my_data->a_part_buf, PART_SIZE, PART_SIZE);
	set_matrix_from_buf(my_data->b_part, my_data->b_part_buf, PART_SIZE, PART_SIZE);
	print_initial_receive();
}

void print_initial_and_control(double **a, double **b){
	double **control = create_empty_matrix(MAT_SIZE, MAT_SIZE);
	printf("A:\n");
	print_matrix(a, MAT_SIZE, MAT_SIZE);
	printf("B:\n");
	print_matrix(b, MAT_SIZE, MAT_SIZE);
	mat_mul_serial(a, b, control, MAT_SIZE, MAT_SIZE, MAT_SIZE);
	printf("Control:\n");
	print_matrix(control, MAT_SIZE, MAT_SIZE);
	free(*control);
	free(control);
}

/*
struct p_data {
	int rank;
	int my_i;
	int my_j;
	double **a_part;
	double **b_part;
	double *a_buf;
	double *b_buf;
	double *a_recv_buf;
	double *b_recv_buf;
};

*/

double **create_matrix_from_buf(double *buf, int m, int n){
	double **result = malloc(sizeof(double) * m);
	int i;
	for (i = 0; i < m; i++) {
		result[i] = &(buf[i * n]);
	}
	return result;
}

void init_local_data(){
	my_data->a_part_buf = malloc(sizeof(double) * PART_SIZE * PART_SIZE);
	my_data->b_part_buf = malloc(sizeof(double) * PART_SIZE * PART_SIZE);
	my_data->a_part = create_matrix_from_buf(my_data->a_part_buf, PART_SIZE, PART_SIZE);
	my_data->b_part = create_matrix_from_buf(my_data->b_part_buf, PART_SIZE, PART_SIZE);
	my_data->c_part = create_empty_matrix(PART_SIZE, PART_SIZE);
	my_data->a_recv_buf = malloc(sizeof(double) * PART_SIZE * PART_SIZE);
	my_data->b_recv_buf = malloc(sizeof(double) * PART_SIZE * PART_SIZE);
	if(my_data->rank >= PART_SIZE){
		my_data->i = my_data->rank / PART_SIZE;
		my_data->j = my_data->rank - (PART_SIZE * my_data->i);
	} else {
		my_data->i = 0;
		my_data->j = my_data->rank;
	}
	// TODO: this should be num grids rather than part size
	// TODO: nicer way to do this
	if(my_data->rank < PART_SIZE){
		my_data->b_dest = (PART_SIZE * PART_SIZE) - (PART_SIZE - my_data->rank);
	} else {
		my_data->b_dest = my_data->rank - PART_SIZE;
	}
	if(my_data->j == 0){
		my_data->a_dest = (PART_SIZE * (my_data->i + 1)) - 1;
	} else {
		my_data->a_dest = (PART_SIZE * my_data->i) + my_data->j - 1;
	}
}

int main(int argc, char *argv[]){
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &(my_data->rank));
	init_local_data();
	if(my_data->rank == 0){
		double **a = create_matrix_with_random_values(MAT_SIZE, MAT_SIZE);
		double **b = create_matrix_with_random_values(MAT_SIZE, MAT_SIZE);
		print_initial_and_control(a, b);
		send_all_initial_parts(a, b);
		my_data->i = 0;
		my_data->j = 0;
		extract_part(a, my_data->a_part, 0, 0);
		extract_part(b, my_data->b_part, 0, 0);
	} else {
		receive_initial_parts();
	}
	MPI_Request request;
	MPI_Status s;
	mat_mul_part(my_data->a_part, my_data->b_part, my_data->c_part, PART_SIZE, PART_SIZE, PART_SIZE);
	//printf("P(%d) sending A to %d and B to %d\n", my_data->rank, my_data->a_dest, my_data->b_dest);
	// Async send A to dest
	if(my_data->a_dest == 6){
		printf("%d sending A to 1\n", my_data->rank);
		int i;
		for(i = 0; i < PART_SIZE * PART_SIZE; i++){
			printf("%f ", my_data->a_part_buf[i]);
		}
		printf("\n");
	}
	MPI_Isend(my_data->a_part_buf, PART_SIZE * PART_SIZE, MPI_DOUBLE, my_data->a_dest, 0, MPI_COMM_WORLD, &request);
	// Sync receive A from source
	MPI_Recv(my_data->a_recv_buf, PART_SIZE * PART_SIZE, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &s);
	MPI_Barrier(MPI_COMM_WORLD);
	// Async send B to dest
	if(my_data->b_dest == 6){
		printf("%d sending B to 1\n", my_data->rank);
		int i;
		for(i = 0; i < PART_SIZE * PART_SIZE; i++){
			printf("%f ", my_data->b_part_buf[i]);
		}
		printf("\n");
	}
	MPI_Isend(my_data->b_part_buf, PART_SIZE * PART_SIZE, MPI_DOUBLE, my_data->b_dest, 0, MPI_COMM_WORLD, &request);
	// Sync receive B from source
	MPI_Recv(my_data->b_recv_buf, PART_SIZE * PART_SIZE, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &s);
	memcpy(my_data->a_part_buf, my_data->a_recv_buf, PART_SIZE * PART_SIZE * sizeof(double));
	memcpy(my_data->b_part_buf, my_data->b_recv_buf, PART_SIZE * PART_SIZE * sizeof(double));
	set_matrix_from_buf(my_data->a_part, my_data->a_part_buf, PART_SIZE, PART_SIZE);
	set_matrix_from_buf(my_data->b_part, my_data->b_part_buf, PART_SIZE, PART_SIZE);
	/**/
	sleep(rand() % (10 + 1 - 2) + 2);
	if(my_data->rank == 6){
		printf("P(%d) received:\n", my_data->rank);
		printf("A:\n");
		print_matrix(my_data->a_part, PART_SIZE, PART_SIZE);
		int i;
		for(i = 0; i < PART_SIZE * PART_SIZE; i++){
			printf("%f ", my_data->a_recv_buf[i]);
		}
		printf("\n");
		printf("B:\n");
		print_matrix(my_data->b_part, PART_SIZE, PART_SIZE);
		for(i = 0; i < PART_SIZE * PART_SIZE; i++){
			printf("%f ", my_data->b_recv_buf[i]);
		}
		printf("\n");
	}

	
	sleep(rand() % (10 + 1 - 2) + 2);
	mat_mul_part(my_data->a_part, my_data->b_part, my_data->c_part, PART_SIZE, PART_SIZE, PART_SIZE);
	printf("Final C for %d:\n", my_data->rank);
	print_matrix(my_data->c_part, PART_SIZE, PART_SIZE);
	
	MPI_Finalize();
	return 0;
}

