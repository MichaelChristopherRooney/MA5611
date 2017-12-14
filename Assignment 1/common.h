// All of the includes we need
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include <mpi.h>

#define PRINT_INITIAL_MATRICES 0
#define USE_CONTROL_MATRIX 1
#define TIME_EXECUTION 1

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

// These will be set shortly after main has been called.
int MAT_SIZE;
int NUM_DIVISIONS;
int NUM_PARTS;
int PART_SIZE;

// These will only be used by process 0.
// The correct A and B parts will be sent by process 0 to each other process.
// CONTROL_MAT is A*B done serially, so we can compare it to the parallel result.
double **A_MAT;
double **B_MAT;
double **CONTROL_MAT;
double **RESULT_MAT;

// Print functions for debugging
void print_matrix(double **mat, int m, int n);
void print_initial_and_control();
void print_initial_send(int dest, int i, int j, int shift, double **a_temp, double **b_temp);
void print_initial_receive();
void print_partial_results();

// Functions for creating and working with matrices
void free_mat(double **mat);
void set_matrix_from_buf(double **mat, double *buf, int m, int n);
double **create_matrix_from_buf(double *buf, int m, int n);
double **create_empty_matrix(int m, int n);
double **create_matrix_with_random_values(int m, int n);
void mat_mul_serial(double **a, double **b, double **c, int l, int m, int n);
void mat_mul_part(double **a, double **b, double **c, int l, int m, int n);
void extract_part(double **source, double **dest, int start_row, int start_col);
void insert_part(double **source, double **dest, int start_row, int start_col);
void create_initial_matrices();
void check_result_matches_control();

// Timing codde
void start_timer();
void end_timer(char *s);
