#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

#define MAT_SIZE 18
#define NUM_DIVISIONS 3
static const int NUM_PARTS = NUM_DIVISIONS * NUM_DIVISIONS;
static const int PART_SIZE = MAT_SIZE / NUM_DIVISIONS;

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
	int a_source;
	int b_source;
};

struct p_data data;
struct p_data *my_data = &data;

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
	double *mat = calloc(sizeof(double),  m * n); // needs calloc - got weird results using malloc
	int i;
	for (i = 0; i < m; i++) {
		result[i] = &(mat[i * n]);
	}
	return result;
}

// TODO: make this random again
double **create_matrix_with_random_values(int m, int n) {
	double **mat = create_empty_matrix(m, n);
	int count = 0;
	int i, j;
	for (i = 0; i < m; i++) {
		for (j = 0; j < n; j++) {
			mat[i][j] = rand() % 5; // numbers in range 0-4
			mat[i][j] -= 2; // adjust range to -2-+2
			//mat[i][j] = count;
			//count++;
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
			//double old = 0.0;
			//printf("P %d, old C[%d][%d] is: %f\n", my_data->rank, i, j, c[i][j]);
			for (k = 0; k < m; k++) {
				sum += (a[i][k] * b[k][j]);
				//printf("P %d, mult %f and %f, adding to %f, new value is %f\n", my_data->rank, a[i][k], b[k][j], old, sum);
				//double filler = 1.0 * a[i][k] * b[k][j];
				//old = sum;
			}
			c[i][j] += sum;
			//printf("P %d, new C[%d][%d] is: %f\n", my_data->rank, i, j, c[i][j]);
		}
	}
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
	free_mat(control);
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
	//printf("B[%d][%d]:\n", shift, j);	
	//print_matrix(b_temp, PART_SIZE, PART_SIZE);
	printf("i: %d, j: %d\n", i, j);
	printf("========================================\n");
}

void send_initial_part(double **a, double **b, double **a_temp, double **b_temp, int i, int j){
	int dest = j + (i*NUM_DIVISIONS);
	int shift = (i + j) % ((int) sqrt(NUM_PARTS));
	extract_part(a, a_temp, i*PART_SIZE, shift*PART_SIZE);
	extract_part(b, b_temp, shift*PART_SIZE, j*PART_SIZE);
	//print_initial_send(dest, i, j, shift, a_temp, b_temp);
	MPI_Send(*a_temp, PART_SIZE * PART_SIZE, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
	MPI_Send(*b_temp, PART_SIZE * PART_SIZE, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
}

// Sends the initial parts to the corresponding destination node.
void send_all_initial_parts(double **a, double **b){
	double **a_temp = create_empty_matrix(PART_SIZE, PART_SIZE);
	double **b_temp = create_empty_matrix(PART_SIZE, PART_SIZE);
	int i, j;
	for(i = 0; i < NUM_DIVISIONS; i++){
		for(j = 0; j < NUM_DIVISIONS; j++){
			if(i == 0 && j == 0){
				continue;
			}
			send_initial_part(a, b, a_temp, b_temp, i, j);
		}
	}
	free_mat(a_temp);
	free_mat(b_temp);
}

// Only process with rank 0 will come here
void prepare_data(){
	double **a = create_matrix_with_random_values(MAT_SIZE, MAT_SIZE);
	double **b = create_matrix_with_random_values(MAT_SIZE, MAT_SIZE);
	print_initial_and_control(a, b);
	send_all_initial_parts(a, b);
	my_data->i = 0;
	my_data->j = 0;
	extract_part(a, my_data->a_part, 0, 0);
	extract_part(b, my_data->b_part, 0, 0);
}

void print_initial_receive(){
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
	//print_initial_receive();
}

// Sets the nodes that this process will receive A and B from.
void set_a_b_source_nodes(){
	int a_source_i, a_source_j, b_source_i, b_source_j;
	a_source_i = my_data->i;
	if(my_data->j == NUM_DIVISIONS - 1){
		a_source_j = 0;
	} else {
		a_source_j = my_data->j + 1;
	}
	b_source_j = my_data->j;
	if(my_data->i == NUM_DIVISIONS - 1){
		b_source_i = 0;
	} else {
		b_source_i = my_data->i + 1;
	}
	my_data->a_source = a_source_j + (a_source_i * NUM_DIVISIONS);
	my_data->b_source = b_source_j + (b_source_i * NUM_DIVISIONS);
}

// Sets the nodes that this process's A and B copies will be sent to.
void set_a_b_dest_nodes(){
	int a_dest_i, a_dest_j, b_dest_i, b_dest_j;
	a_dest_i = my_data->i;
	b_dest_j = my_data->j;
	if(my_data->j == 0){
		a_dest_j = NUM_DIVISIONS - 1;
	} else {
		a_dest_j = my_data->j - 1;
	}
	if(my_data->i == 0){
		b_dest_i = NUM_DIVISIONS - 1;
	} else {
		b_dest_i = my_data->i - 1;
	}
	my_data->a_dest = a_dest_j + (a_dest_i * NUM_DIVISIONS);
	my_data->b_dest = b_dest_j + (b_dest_i * NUM_DIVISIONS);
}

void set_i_j_from_rank(){
	if(my_data->rank >= NUM_DIVISIONS){
		my_data->i = my_data->rank / NUM_DIVISIONS;
		my_data->j = my_data->rank - (NUM_DIVISIONS * my_data->i);
	} else {
		my_data->i = 0;
		my_data->j = my_data->rank;
	}
}

void create_buffers(){
	my_data->a_part_buf = malloc(sizeof(double) * PART_SIZE * PART_SIZE);
	my_data->b_part_buf = malloc(sizeof(double) * PART_SIZE * PART_SIZE);
	my_data->a_part = create_matrix_from_buf(my_data->a_part_buf, PART_SIZE, PART_SIZE);
	my_data->b_part = create_matrix_from_buf(my_data->b_part_buf, PART_SIZE, PART_SIZE);
	my_data->c_part = create_empty_matrix(PART_SIZE, PART_SIZE);
	my_data->a_recv_buf = malloc(sizeof(double) * PART_SIZE * PART_SIZE);
	my_data->b_recv_buf = malloc(sizeof(double) * PART_SIZE * PART_SIZE);
}

void init_local_data(){
	create_buffers();
	set_i_j_from_rank();
	set_a_b_dest_nodes();
	set_a_b_source_nodes();
}

void cleanup(){
	free_mat(my_data->a_part);
	free_mat(my_data->b_part);
	free_mat(my_data->c_part);
	free(my_data->a_recv_buf);
	free(my_data->b_recv_buf);
}

void print_partial_results(){
	int i;
	for(i = 0; i < NUM_PARTS; i++) {
		MPI_Barrier(MPI_COMM_WORLD);
		if (i == my_data->rank) {
			printf("Final C for %d:\n", my_data->rank);
			print_matrix(my_data->c_part, PART_SIZE, PART_SIZE);
			fflush(stdout);			
		}
	}	
}

// Note: each process should have already multiplied its initial A and B parts before coming here.
// Send A and B parts to the correct processes, and also receives A and B from the correct processes.
// Multiplies the received A and B parts
void do_iteration(){
	MPI_Status s;
	MPI_Request req_1;
	MPI_Request req_2;
	MPI_Isend(my_data->a_part_buf, PART_SIZE * PART_SIZE, MPI_DOUBLE, my_data->a_dest, 4, MPI_COMM_WORLD, &req_1);
	MPI_Recv(my_data->a_recv_buf, PART_SIZE * PART_SIZE, MPI_DOUBLE, my_data->a_source, 4, MPI_COMM_WORLD, &s);
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Isend(my_data->b_part_buf, PART_SIZE * PART_SIZE, MPI_DOUBLE, my_data->b_dest, 5, MPI_COMM_WORLD, &req_2);
	MPI_Recv(my_data->b_recv_buf, PART_SIZE * PART_SIZE, MPI_DOUBLE, my_data->b_source, 5, MPI_COMM_WORLD, &s);
	MPI_Barrier(MPI_COMM_WORLD);
	memcpy(my_data->a_part_buf, my_data->a_recv_buf, PART_SIZE * PART_SIZE * sizeof(double));
	memcpy(my_data->b_part_buf, my_data->b_recv_buf, PART_SIZE * PART_SIZE * sizeof(double));
	MPI_Barrier(MPI_COMM_WORLD);
	mat_mul_part(my_data->a_part, my_data->b_part, my_data->c_part, PART_SIZE, PART_SIZE, PART_SIZE);
	MPI_Barrier(MPI_COMM_WORLD);
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

// Only process 0 should enter here.
// Receives partial results from all other processes and combines them.
void collect_final_result(){
	MPI_Status s;
	int i, j;
	//double *recv_buf = malloc(sizeof(double) * PART_SIZE * PART_SIZE);
	//double **recv_mat = create_matrix_from_buf(recv_buf, PART_SIZE, PART_SIZE);
	double **recv = create_empty_matrix(PART_SIZE, PART_SIZE);
	double **result = create_empty_matrix(MAT_SIZE, MAT_SIZE);
	for(i = 0; i < NUM_DIVISIONS; i++){
		for(j = 0; j < NUM_DIVISIONS; j++){
			if(i == 0 && j == 0){
				insert_part(my_data->c_part, result, 0, 0);
				continue; 
			}
			MPI_Barrier(MPI_COMM_WORLD);
			int recv_source = j + (i*NUM_DIVISIONS);
			MPI_Recv(*recv, PART_SIZE * PART_SIZE, MPI_DOUBLE, recv_source, 4, MPI_COMM_WORLD, &s);
			insert_part(recv, result, i * PART_SIZE , j * PART_SIZE);
		}
	}
	printf("Final C is:\n");
	print_matrix(result, MAT_SIZE, MAT_SIZE);
	free_mat(recv);
}

void send_final_result(){
	int i;
	for(i = 1; i < NUM_PARTS; i++){
		MPI_Barrier(MPI_COMM_WORLD);
		if(my_data->rank == i){
			MPI_Send(*my_data->c_part, PART_SIZE * PART_SIZE, MPI_DOUBLE, 0, 4, MPI_COMM_WORLD);
		}
	}
}

// TODO:
// investigate using transpose
// remove un-needed barriers
// further cleanup
// use ANY_TAG where possible
int main(int argc, char *argv[]){
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &(my_data->rank));
	init_local_data();
	if(my_data->rank == 0){
		prepare_data();
	} else {
		receive_initial_parts();
	}
	mat_mul_part(my_data->a_part, my_data->b_part, my_data->c_part, PART_SIZE, PART_SIZE, PART_SIZE);
	MPI_Barrier(MPI_COMM_WORLD);
	int i;
	for(i = 1; i < NUM_DIVISIONS; i++){
		do_iteration();
	}
	if(my_data->rank == 0){
		collect_final_result();
	} else {
		send_final_result();	
	}
	MPI_Finalize();
	return 0;
}

