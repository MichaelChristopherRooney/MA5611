#include "common.h"

static void init_mpi_cart_grid(){
	NCOLS = 12;
	NROWS = 12;
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
}

static void init(int argc, char *argv[]){
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &RANK);
	MPI_Comm_size(MPI_COMM_WORLD, &NUM_NODES);
	init_mpi_cart_grid();
	init_grid();
}

static void send_and_recv_left_col(MPI_Datatype col_vec){
	// Async send
	MPI_Request req;
	int dest_rank;
	int dest_coords[2] = { LOCAL_X_COORD - 1, LOCAL_Y_COORD };
	MPI_Cart_rank(CART_COMM, dest_coords, &dest_rank);
	MPI_Isend(&grid[1][1], 1, col_vec, dest_rank, 0, CART_COMM, &req);
	// Then recv
	MPI_Status status;
	int source_rank;
	int source_coords[2] = { LOCAL_X_COORD - 1, LOCAL_Y_COORD };
	MPI_Cart_rank(CART_COMM, source_coords, &source_rank);
	MPI_Recv(&grid[1][0], 1, col_vec, source_rank, MPI_ANY_TAG, CART_COMM, &status);
}

static void send_and_recv_right_col(MPI_Datatype col_vec){
	// Async send
	MPI_Request req;
	int dest_rank;
	int dest_coords[2] = { LOCAL_X_COORD + 1, LOCAL_Y_COORD };
	MPI_Cart_rank(CART_COMM, dest_coords, &dest_rank);
	MPI_Isend(&grid[1][LOCAL_NCOLS - 2], 1, col_vec, dest_rank, 0, CART_COMM, &req);
	// Then recv
	MPI_Status status;
	int source_rank;
	int source_coords[2] = { LOCAL_X_COORD + 1, LOCAL_Y_COORD };
	MPI_Cart_rank(CART_COMM, source_coords, &source_rank);
	MPI_Recv(&grid[1][LOCAL_NCOLS-1], 1, col_vec, source_rank, MPI_ANY_TAG, CART_COMM, &status);
}

static void send_and_recv_top_row(){
	// Async send
	MPI_Request req;
	int dest_rank;
	int dest_coords[2] = { LOCAL_X_COORD, LOCAL_Y_COORD - 1 };
	MPI_Cart_rank(CART_COMM, dest_coords, &dest_rank);
	MPI_Isend(&grid[1][1], LOCAL_NCOLS - 2, MPI_DOUBLE, dest_rank, 0, CART_COMM, &req);
	// Then recv
	MPI_Status status;
	int source_rank;
	int source_coords[2] = { LOCAL_X_COORD, LOCAL_Y_COORD - 1 };
	MPI_Cart_rank(CART_COMM, source_coords, &source_rank);
	MPI_Recv(&grid[0][1], LOCAL_NCOLS - 1, MPI_DOUBLE, source_rank, MPI_ANY_TAG, CART_COMM, &status);
}

static void send_and_recv_bottom_row(){
	// Async send
	MPI_Request req;
	int dest_rank;
	int dest_coords[2] = { LOCAL_X_COORD, LOCAL_Y_COORD + 1 };
	MPI_Cart_rank(CART_COMM, dest_coords, &dest_rank);
	MPI_Isend(&grid[LOCAL_NROWS - 2][1], LOCAL_NCOLS - 2, MPI_DOUBLE, dest_rank, 0, CART_COMM, &req);
	// Then recv
	MPI_Status status;
	int source_rank;
	int source_coords[2] = { LOCAL_X_COORD, LOCAL_Y_COORD + 1 };
	MPI_Cart_rank(CART_COMM, source_coords, &source_rank);
	MPI_Recv(&grid[LOCAL_NROWS - 1][1], LOCAL_NCOLS - 1, MPI_DOUBLE, source_rank, MPI_ANY_TAG, CART_COMM, &status);
}

// TODO: only create data type once at start
// TODO: more send/recv pairs
// TODO: break this up into smaller functions
// TODO: need for separate dest/source coords?
static void exchange(){
	MPI_Datatype col_vec;
	MPI_Type_vector(LOCAL_NROWS - 2, 1, LOCAL_NCOLS, MPI_DOUBLE, &col_vec);
	MPI_Type_commit(&col_vec);
	if(LOCAL_Y_COORD != 0){
		send_and_recv_top_row();
	}
	if(LOCAL_Y_COORD != MPI_CART_DIMS[Y_INDEX] - 1){
		send_and_recv_bottom_row();
	}
	if(LOCAL_X_COORD != 0){
		send_and_recv_left_col(col_vec);
	}
	if(LOCAL_X_COORD != MPI_CART_DIMS[X_INDEX] - 1){
		send_and_recv_right_col(col_vec);
	}
	MPI_Type_free(&col_vec);
}

static void get_iter_limits(int *col_start, int *col_end, int *row_start, int *row_end){
	if(LOCAL_X_COORD == 0){ // leftmost in x dim
		*col_start = 2;
		*col_end = LOCAL_NCOLS - 1;
	} else if(LOCAL_X_COORD == MPI_CART_DIMS[X_INDEX] - 1){ // rightmost in x dim
		*col_start = 1;
		*col_end = LOCAL_NCOLS - 2;
	} else { // not at either x edge
		*col_start = 1;
		*col_end = LOCAL_NCOLS - 1;
	}
	if(LOCAL_Y_COORD == 0){ // topmost in y dim
		*row_start = 2;
		*row_end = LOCAL_NROWS - 1;
	} else if(LOCAL_Y_COORD == MPI_CART_DIMS[Y_INDEX] - 1){ // bottommost in y dim
		*row_start = 1;
		*row_end = LOCAL_NROWS - 2;
	} else { // not at either y edge
		*row_start = 1;
		*row_start = LOCAL_NROWS - 1;
	}
}

static void do_iteration(){
	int col_start, col_end, row_start, row_end;
	get_iter_limits(&col_start, &col_end, &row_start, &row_end);
	int i, n;
	for(i = row_start; i < row_end; i++){
		for(n = col_start; n < col_end; n++){
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
