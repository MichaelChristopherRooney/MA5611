#include "common.h"

static void init_mpi_cart_grid(){
	NCOLS = 18;
	NROWS = 18;
	int dims_tmp[2] = { 3, 3 };
	int pbc[2] = { 0, 0 };
	MPI_Dims_create(NUM_NODES, 2, dims_tmp);
	MPI_CART_DIMS[X_INDEX] = dims_tmp[X_INDEX];
	MPI_CART_DIMS[Y_INDEX] = dims_tmp[Y_INDEX];
	// TODO: ensure this works for different sizes
	LOCAL_NROWS = (NROWS / MPI_CART_DIMS[Y_INDEX]) + 2;
	LOCAL_NCOLS = (NCOLS / MPI_CART_DIMS[X_INDEX]) + 2;
	MPI_Cart_create(MPI_COMM_WORLD, 2, MPI_CART_DIMS, pbc, 0, &CART_COMM);
	MPI_Comm_rank(MPI_COMM_WORLD, &CART_RANK);
	int coords[2];
	MPI_Cart_coords(CART_COMM, RANK, 2, coords);
	LOCAL_X_COORD = coords[0];
	LOCAL_Y_COORD = coords[1];
	MPI_Type_vector(LOCAL_NROWS - 2, 1, LOCAL_NCOLS, MPI_DOUBLE, &col_vec);
	MPI_Type_commit(&col_vec);
}

static void init(int argc, char *argv[]){
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &RANK);
	MPI_Comm_size(MPI_COMM_WORLD, &NUM_NODES);
	init_mpi_cart_grid();
	init_grid();
}

static void send_and_recv_left_col(MPI_Datatype col_vec){
	int left_rank;
	int left_coords[2] = { LOCAL_X_COORD - 1, LOCAL_Y_COORD };
	MPI_Cart_rank(CART_COMM, left_coords, &left_rank);
	MPI_Request req;
	MPI_Isend(&grid[1][1], 1, col_vec, left_rank, 0, CART_COMM, &req);
	MPI_Irecv(&grid[1][0], 1, col_vec, left_rank, MPI_ANY_TAG, CART_COMM, &req);
}

static void send_and_recv_right_col(MPI_Datatype col_vec){
	int right_rank;
	int right_coords[2] = { LOCAL_X_COORD + 1, LOCAL_Y_COORD };
	MPI_Cart_rank(CART_COMM, right_coords, &right_rank);
	MPI_Request req;
	MPI_Isend(&grid[1][LOCAL_NCOLS - 2], 1, col_vec, right_rank, 0, CART_COMM, &req);
	MPI_Irecv(&grid[1][LOCAL_NCOLS-1], 1, col_vec, right_rank, MPI_ANY_TAG, CART_COMM, &req);
}

static void send_and_recv_top_row(){
	int top_rank;
	int top_coords[2] = { LOCAL_X_COORD, LOCAL_Y_COORD - 1 };
	MPI_Cart_rank(CART_COMM, top_coords, &top_rank);
	MPI_Request req;
	MPI_Isend(&grid[1][1], LOCAL_NCOLS - 2, MPI_DOUBLE, top_rank, 0, CART_COMM, &req);
	MPI_Irecv(&grid[0][1], LOCAL_NCOLS - 1, MPI_DOUBLE, top_rank, MPI_ANY_TAG, CART_COMM, &req);
}

static void send_and_recv_bottom_row(){
	int bottom_rank;
	int bottom_coords[2] = { LOCAL_X_COORD, LOCAL_Y_COORD + 1 };
	MPI_Cart_rank(CART_COMM, bottom_coords, &bottom_rank);
	MPI_Request req;
	MPI_Isend(&grid[LOCAL_NROWS - 2][1], LOCAL_NCOLS - 2, MPI_DOUBLE, bottom_rank, 0, CART_COMM, &req);
	MPI_Irecv(&grid[LOCAL_NROWS - 1][1], LOCAL_NCOLS - 1, MPI_DOUBLE, bottom_rank, MPI_ANY_TAG, CART_COMM, &req);
}

static void exchange(){
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
	MPI_Barrier(CART_COMM);
}

static void get_iter_limits(int *col_start, int *col_end, int *row_start, int *row_end){
	if(MPI_CART_DIMS[X_INDEX] > 1){
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
	} else {
		*col_start = 2;
		*col_end = LOCAL_NCOLS - 2;
	}
	if(MPI_CART_DIMS[Y_INDEX] > 1){
		if(LOCAL_Y_COORD == 0){ // topmost in y dim
			*row_start = 2;
			*row_end = LOCAL_NROWS - 1;
		} else if(LOCAL_Y_COORD == MPI_CART_DIMS[Y_INDEX] - 1){ // bottommost in y dim
			*row_start = 1;
			*row_end = LOCAL_NROWS - 2;
		} else { // not at either y edge
			*row_start = 1;
			*row_end = LOCAL_NROWS - 1;
		}
	} else {
		*row_start = 2;
		*row_end = LOCAL_NROWS - 2;
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

// TODO: use memcpy
static void insert_final_result(double **buf, int x, int y){
	int row_start = (LOCAL_NROWS - 2) * y;
	int col_start = (LOCAL_NCOLS - 2) * x;
	int i, n;
	for(i = 0; i < (LOCAL_NROWS - 2); i++){
		for(n = 0; n < (LOCAL_NCOLS - 2); n++){
			final_grid[i + row_start][n + col_start] = buf[i + 1][n + 1];
		}
	}
}

// Only rank 0 should ever be here
static void receive_final_results(){
	insert_final_result(grid, 0, 0);
	int i;
	for(i = 1; i < NUM_NODES; i++){
		MPI_Status status;
		MPI_Recv(&recv_grid[0][0], LOCAL_NROWS * LOCAL_NCOLS, MPI_DOUBLE, i, MPI_ANY_TAG, CART_COMM, &status);
		int coords[2] = { 0, 0 };
		MPI_Cart_coords(CART_COMM, i, 2, coords);
		//printf("Rank 0 received from %d, with x = %d and y = %d\n", i, coords[0], coords[1]);
		insert_final_result(recv_grid, coords[0], coords[1]);
		//print_recv_grid();
		MPI_Barrier(MPI_COMM_WORLD);
	}
}

// Everyone but rank 0 should be here
static void send_final_results(){
	int i;
	for(i = 1; i < NUM_NODES; i++){
		if(CART_RANK == i){
			MPI_Send(&grid[0][0], LOCAL_NROWS * LOCAL_NCOLS, MPI_DOUBLE, 0, 0, CART_COMM);
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}
}


static void cleanup(){
	MPI_Type_free(&col_vec);
	free_grid(grid);
	free_grid(prev_grid);
	if(CART_RANK == 0){
		free_grid(recv_grid);
		free_grid(final_grid);
	}
}

int main(int argc, char *argv[]){
	init(argc, argv);
	int i;
	for(i = 0; i < 100; i++){
		exchange();
		do_iteration();
	}
	//print_all_grids();
	if(CART_RANK == 0){
		receive_final_results();
		print_final_grid();
	} else {
		send_final_results();
	}
	cleanup();
	MPI_Finalize();
	return 0;
}
