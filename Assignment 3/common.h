#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

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

// The size of the grid that the individual node is dealing with.
int LOCAL_NCOLS;
int LOCAL_NROWS;

MPI_Comm CART_COMM;

// Grid functions and data
double **grid;
double **prev_grid;
void print_all_grids();
void init_grid();
