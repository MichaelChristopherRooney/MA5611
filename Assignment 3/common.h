#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define NUM_ITER 100

// The size of the overall grid.
int NCOLS;
int NROWS;

// The size of the cart grid of MPI nodes in each dimension.
// [0] gives x dim, [1] gives y dim
#define X_INDEX 0
#define Y_INDEX 1
int MPI_CART_DIMS[2];

// The local nodes location in the MPI grid
int LOCAL_X_COORD;
int LOCAL_Y_COORD;

// Overall number of MPI nodes
int NUM_NODES;

int RANK;
int CART_RANK;

// The size of the grid that the individual node is dealing with.
int LOCAL_NCOLS;
int LOCAL_NROWS;

MPI_Comm CART_COMM;

MPI_Datatype col_vec;

// Grid functions and data
double **grid;
double **prev_grid;
void print_all_grids();
void init_grid();
void free_grid(double **grid);
// The following should only be called by rank 0 in the cart topology
void print_final_grid();
void print_recv_grid();
double **final_grid;
double **recv_grid;

void compare();

