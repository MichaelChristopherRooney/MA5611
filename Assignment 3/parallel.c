#include "common.h"

// Note: only allocates buffers if they are needed.
// Example: does not allocate a left recv buffer if the local nodes is touching
// the left side of the cart grid.
static void init_recv_bufs(){
	if(LOCAL_Y_COORD != 0){
		//printf("Rank %d: allocating top buf\n", RANK);
		TOP_RECV_BUF = calloc(LOCAL_NCOLS, sizeof(double));
	}
	if(LOCAL_Y_COORD != MPI_CART_DIMS[Y_INDEX] - 1){
		//printf("Rank %d: allocating bottom buf\n", RANK);
		BOTTOM_RECV_BUF = calloc(LOCAL_NCOLS, sizeof(double));
	}
	if(LOCAL_X_COORD != 0){
		//printf("Rank %d: allocating left buf\n", RANK);
		LEFT_RECV_BUF = calloc(LOCAL_NROWS, sizeof(double));
	}
	if(LOCAL_X_COORD != MPI_CART_DIMS[X_INDEX] - 1){
		//printf("Rank %d: allocating right buf\n", RANK);
		RIGHT_RECV_BUF = calloc(LOCAL_NROWS, sizeof(double));
	}
}

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
	init_recv_bufs();
	init_grid();
}

// TODO: only create data type once at start
// TODO: more send/recv pairs
// TODO: vector recv is currently overwriting memory elsewhere
static void exchange(){
	MPI_Datatype col_vec;
	MPI_Type_vector(LOCAL_NROWS, 1, LOCAL_NCOLS, MPI_DOUBLE, &col_vec);
	MPI_Type_commit(&col_vec);
	if(LOCAL_Y_COORD != 0){
		printf("hey\n");
	}
	if(LOCAL_Y_COORD != MPI_CART_DIMS[Y_INDEX] - 1){
		printf("hey2\n");
	}
	if(LOCAL_X_COORD != 0){
		//printf("Rank %d: want to send to left and receive from left\n", RANK);
		MPI_Request req;
		int dest_rank;
		int dest_coords[2] = { LOCAL_X_COORD - 1, LOCAL_Y_COORD };
		MPI_Cart_rank(CART_COMM, dest_coords, &dest_rank);
		MPI_Isend(&grid[0][0], 1, col_vec, dest_rank, 0, CART_COMM, &req);
	}
	if(LOCAL_X_COORD != MPI_CART_DIMS[X_INDEX] - 1){
		MPI_Status status;
		int source_rank;
		int source_coords[2] = { LOCAL_X_COORD + 1, LOCAL_Y_COORD };
		MPI_Cart_rank(CART_COMM, source_coords, &source_rank);
		MPI_Recv(RIGHT_RECV_BUF, 1, col_vec, source_rank, MPI_ANY_TAG, CART_COMM, &status);
		printf("Right recv buf: \n");
		int i;
		for(i = 0; i < LOCAL_NROWS * 10; i++){
			printf("%f, ", RIGHT_RECV_BUF[i]);
		}
		printf("\n");
	}
	MPI_Type_free(&col_vec);
}

int main(int argc, char *argv[]){
	init(argc, argv);
	exchange();	
	//print_all_grids();
	MPI_Finalize();
	return 0;
}
