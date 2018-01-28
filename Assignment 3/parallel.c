#include "common.h"

static void init_mpi_cart_grid(){
	NCOLS = 10;
	NROWS = 10;
	int dims_tmp[2] = { 0, 0 };
	int pbc[2] = { 0, 0 };
	MPI_Dims_create(NUM_NODES, 2, dims_tmp);
	MPI_CART_DIMS[X_INDEX] = dims_tmp[X_INDEX];
	MPI_CART_DIMS[Y_INDEX] = dims_tmp[Y_INDEX];
	// TODO: ensure this works for different sizes
	LOCAL_NROWS = NROWS / MPI_CART_DIMS[Y_INDEX];
	LOCAL_NCOLS = NCOLS / MPI_CART_DIMS[X_INDEX];
	MPI_Cart_create(MPI_COMM_WORLD, 2, MPI_CART_DIMS, pbc, 0, &CART_COMM);
	int coords[2];
	MPI_Cart_coords(CART_COMM, RANK, 2, coords);
	LOCAL_X_COORD = coords[0];
	LOCAL_Y_COORD = coords[1];
}

static void init(int argc, char *argv[]){
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &RANK);
	MPI_Comm_size(MPI_COMM_WORLD, &NUM_NODES);
	init_mpi_cart_grid();
}

int main(int argc, char *argv[]){
	init(argc, argv);
	init_grid();
	print_all_grids();
	MPI_Finalize();
	return 0;
}
