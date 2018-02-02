#include "common.h"

static void send_and_recv_left_col(MPI_Datatype col_vec){
	int left_rank;
	int left_coords[2] = { LOCAL_X_COORD - 1, LOCAL_Y_COORD };
	MPI_Cart_rank(CART_COMM, left_coords, &left_rank);
	MPI_Request req;
	MPI_Status status;
	MPI_Isend(&grid[1][1], 1, col_vec, left_rank, 0, CART_COMM, &req);
	MPI_Recv(&grid[1][0], 1, col_vec, left_rank, MPI_ANY_TAG, CART_COMM, &status);
}

static void send_and_recv_right_col(MPI_Datatype col_vec){
	int right_rank;
	int right_coords[2] = { LOCAL_X_COORD + 1, LOCAL_Y_COORD };
	MPI_Cart_rank(CART_COMM, right_coords, &right_rank);
	MPI_Request req;
	MPI_Status status;
	MPI_Isend(&grid[1][LOCAL_NCOLS - 2], 1, col_vec, right_rank, 0, CART_COMM, &req);
	MPI_Recv(&grid[1][LOCAL_NCOLS-1], 1, col_vec, right_rank, MPI_ANY_TAG, CART_COMM, &status);
}

static void send_and_recv_top_row(){
	int top_rank;
	int top_coords[2] = { LOCAL_X_COORD, LOCAL_Y_COORD - 1 };
	MPI_Cart_rank(CART_COMM, top_coords, &top_rank);
	MPI_Request req;
	MPI_Status status;
	MPI_Isend(&grid[1][1], LOCAL_NCOLS - 2, MPI_DOUBLE, top_rank, 0, CART_COMM, &req);
	MPI_Recv(&grid[0][1], LOCAL_NCOLS - 1, MPI_DOUBLE, top_rank, MPI_ANY_TAG, CART_COMM, &status);
}

static void send_and_recv_bottom_row(){
	int bottom_rank;
	int bottom_coords[2] = { LOCAL_X_COORD, LOCAL_Y_COORD + 1 };
	MPI_Cart_rank(CART_COMM, bottom_coords, &bottom_rank);
	MPI_Request req;
	MPI_Status status;
	MPI_Isend(&grid[LOCAL_NROWS - 2][1], LOCAL_NCOLS - 2, MPI_DOUBLE, bottom_rank, 0, CART_COMM, &req);
	MPI_Recv(&grid[LOCAL_NROWS - 1][1], LOCAL_NCOLS - 1, MPI_DOUBLE, bottom_rank, MPI_ANY_TAG, CART_COMM, &status);
}

void exchange(){
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
}

// Copy a node's partial result grid into the final result grid.
// Use the node's x and y coords to decide where the partial result grid should
// be in the final result grid.
// Only rank 0 should ever be here
static void insert_final_result(double **buf, int x, int y){
	int row_start = (LOCAL_NROWS - 2) * y;
	int col_start = (LOCAL_NCOLS - 2) * x;
	int i;
	for(i = 0; i < (LOCAL_NROWS - 2); i++){
		// copy the row
		memcpy(&final_grid[i + row_start][col_start], &buf[i + 1][1], (LOCAL_NCOLS - 2) * sizeof(double));
	}
}

// Only rank 0 should ever be here
void receive_final_results(){
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
void send_final_results(){
	int i;
	for(i = 1; i < NUM_NODES; i++){
		if(CART_RANK == i){
			MPI_Send(&grid[0][0], LOCAL_NROWS * LOCAL_NCOLS, MPI_DOUBLE, 0, 0, CART_COMM);
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}
}

