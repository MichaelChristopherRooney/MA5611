#include "common.h"

static void init_mpi_cart_grid(){
	NCOLS = 9;
	NROWS = 9;
	int dims_tmp[2] = { 0, 0 };
	int pbc[2] = { 0, 0 };
	MPI_Dims_create(NUM_NODES, 2, dims_tmp);
	MPI_CART_DIMS[X_INDEX] = dims_tmp[X_INDEX];
	MPI_CART_DIMS[Y_INDEX] = dims_tmp[Y_INDEX];
	// TODO: ensure this works for different sizes
	LOCAL_NROWS = (NROWS / MPI_CART_DIMS[Y_INDEX]) + 2;
	LOCAL_NCOLS = (NCOLS / MPI_CART_DIMS[X_INDEX]) + 2;
	MPI_Cart_create(MPI_COMM_WORLD, 2, MPI_CART_DIMS, pbc, 0, &CART_COMM);
	int coords[2];
	MPI_Cart_coords(CART_COMM, RANK, 2, coords);
	LOCAL_X_COORD = coords[0];
	LOCAL_Y_COORD = coords[1];
	printf("Rank %d: x = %d, y = %d\n", RANK, LOCAL_X_COORD, LOCAL_Y_COORD);
}

static void init(int argc, char *argv[]){
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &RANK);
	MPI_Comm_size(MPI_COMM_WORLD, &NUM_NODES);
	init_mpi_cart_grid();
	init_grid();
}

// TODO: only create data type once at start
// TODO: more send/recv pairs
// TODO: break this up into smaller functions
static void exchange(){
	MPI_Datatype col_vec;
	MPI_Type_vector(LOCAL_NROWS - 2, 1, LOCAL_NCOLS, MPI_DOUBLE, &col_vec);
	MPI_Type_commit(&col_vec);
	if(LOCAL_Y_COORD != 0){
		printf("hey\n");
	}
	if(LOCAL_Y_COORD != MPI_CART_DIMS[Y_INDEX] - 1){
		printf("hey2\n");
	}
	if(LOCAL_X_COORD != 0){
		// Send leftmost column to the node to our left in the topology
		MPI_Request req;
		int dest_rank;
		int dest_coords[2] = { LOCAL_X_COORD - 1, LOCAL_Y_COORD };
		MPI_Cart_rank(CART_COMM, dest_coords, &dest_rank);
		MPI_Isend(&grid[1][1], 1, col_vec, dest_rank, 0, CART_COMM, &req);
		// Receive left temp column from the node to our left in the topology
		MPI_Status status;
		int source_rank;
		int source_coords[2] = { LOCAL_X_COORD - 1, LOCAL_Y_COORD };
		MPI_Cart_rank(CART_COMM, source_coords, &source_rank);
		MPI_Recv(&grid[1][0], 1, col_vec, source_rank, MPI_ANY_TAG, CART_COMM, &status);
	}
	if(LOCAL_X_COORD != MPI_CART_DIMS[X_INDEX] - 1){
		// Send rightmost column to the node to our right in the topology
		MPI_Request req;
		int dest_rank;
		int dest_coords[2] = { LOCAL_X_COORD + 1, LOCAL_Y_COORD };
		MPI_Cart_rank(CART_COMM, dest_coords, &dest_rank);
		MPI_Isend(&grid[1][LOCAL_NCOLS - 2], 1, col_vec, dest_rank, 0, CART_COMM, &req);
		// Receive right temp column from the node to our right in the topology
		MPI_Status status;
		int source_rank;
		int source_coords[2] = { LOCAL_X_COORD + 1, LOCAL_Y_COORD };
		MPI_Cart_rank(CART_COMM, source_coords, &source_rank);
		MPI_Recv(&grid[1][LOCAL_NCOLS-1], 1, col_vec, source_rank, MPI_ANY_TAG, CART_COMM, &status);
	}
	MPI_Type_free(&col_vec);
}

static void do_iteration(){
	int col_start, col_end;
	if(LOCAL_X_COORD == 0){ // leftmost in x dim
		col_start = 2;
		col_end = LOCAL_NCOLS - 1;
	} else if(LOCAL_X_COORD == MPI_CART_DIMS[X_INDEX] - 1){ // rightmost in x dim
		col_start = 1;
		col_end = LOCAL_NCOLS - 2;
	} else { // not at either x edge
		col_start = 1;
		col_end = LOCAL_NCOLS - 1;
	}
	int i, n;
	for(i = 2; i < LOCAL_NROWS - 2; i++){
		for(n = col_start; n < col_end; n++){
			//printf("Rank %d: i = %d, n = %d\n", RANK, i, n);
			prev_grid[i][n] = (grid[i-1][n] + grid[i+1][n] + grid[i][n-1] + grid[i][n+1]) / 4.0;
		}
	}
	double **temp = grid;
	grid = prev_grid;
	prev_grid = temp;
}

int main(int argc, char *argv[]){
	init(argc, argv);
	//print_all_grids();
	int i;
	for(i = 0; i < 100; i++){
		exchange();
		do_iteration();
	}
	print_all_grids();
	MPI_Finalize();
	return 0;
}
