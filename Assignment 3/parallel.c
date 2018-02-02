#include "common.h"

static int ITER_COL_START;
static int ITER_COL_END;
static int ITER_ROW_START;
static int ITER_ROW_END;

// Figures out where a node should start/end when iterating over the local grid.
// For example: if the node is leftmost in the topology it should not change any 
// values in its leftmost column as these are fixed.
static void set_iter_limits(){
	if(MPI_CART_DIMS[X_INDEX] > 1){
		if(LOCAL_X_COORD == 0){ // leftmost in x dim
			ITER_COL_START = 2;
			ITER_COL_END = LOCAL_NCOLS - 1;
		} else if(LOCAL_X_COORD == MPI_CART_DIMS[X_INDEX] - 1){ // rightmost in x dim
			ITER_COL_START = 1;
			ITER_COL_END = LOCAL_NCOLS - 2;
		} else { // not at either x edge
			ITER_COL_START = 1;
			ITER_COL_END = LOCAL_NCOLS - 1;
		}
	} else {
		ITER_COL_START = 2;
		ITER_COL_END = LOCAL_NCOLS - 2;
	}
	if(MPI_CART_DIMS[Y_INDEX] > 1){
		if(LOCAL_Y_COORD == 0){ // topmost in y dim
			ITER_ROW_START = 2;
			ITER_ROW_END = LOCAL_NROWS - 1;
		} else if(LOCAL_Y_COORD == MPI_CART_DIMS[Y_INDEX] - 1){ // bottommost in y dim
			ITER_ROW_START = 1;
			ITER_ROW_END = LOCAL_NROWS - 2;
		} else { // not at either y edge
			ITER_ROW_START = 1;
			ITER_ROW_END = LOCAL_NROWS - 1;
		}
	} else {
		ITER_ROW_START = 2;
		ITER_ROW_END = LOCAL_NROWS - 2;
	}
}

// Note: the x/y dimension value will be overwritten here if 0 was passed as an arg
static void create_topology(){
	MPI_Dims_create(NUM_NODES, 2, MPI_CART_DIMS);
	int pbc[2] = { 0, 0 };
	MPI_Cart_create(MPI_COMM_WORLD, 2, MPI_CART_DIMS, pbc, 0, &CART_COMM);
}

static void init_local_topology_data(){
	MPI_Comm_rank(MPI_COMM_WORLD, &CART_RANK);
	int coords[2];
	MPI_Cart_coords(CART_COMM, RANK, 2, coords);
	LOCAL_X_COORD = coords[0];
	LOCAL_Y_COORD = coords[1];
	LOCAL_NROWS = (NROWS / MPI_CART_DIMS[Y_INDEX]) + 2;
	LOCAL_NCOLS = (NCOLS / MPI_CART_DIMS[X_INDEX]) + 2;
	MPI_Type_vector(LOCAL_NROWS - 2, 1, LOCAL_NCOLS, MPI_DOUBLE, &col_vec);
	MPI_Type_commit(&col_vec);
}

static void read_args(int argc, char *argv[]){
	NCOLS = atoi(argv[1]);
	NROWS = atoi(argv[2]);
	MPI_CART_DIMS[X_INDEX] = atoi(argv[3]);
	MPI_CART_DIMS[Y_INDEX] = atoi(argv[4]);
}

static void init(int argc, char *argv[]){
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &RANK);
	MPI_Comm_size(MPI_COMM_WORLD, &NUM_NODES);
	read_args(argc, argv);
	create_topology();
	init_local_topology_data();
	init_grid();
	set_iter_limits();
}

static void do_iteration(){
	int i, n;
	for(i = ITER_ROW_START; i < ITER_ROW_END; i++){
		for(n = ITER_COL_START; n < ITER_COL_END; n++){
			prev_grid[i][n] = (grid[i-1][n] + grid[i+1][n] + grid[i][n-1] + grid[i][n+1]) / 4.0;
		}
	}
	double **temp = grid;
	grid = prev_grid;
	prev_grid = temp;
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
	for(i = 0; i < NUM_ITER; i++){
		exchange();
		do_iteration();
	}
	if(CART_RANK == 0){
		receive_final_results();
		//print_final_grid();
		//compare();
	} else {
		send_final_results();
	}	
	cleanup();
	MPI_Finalize();
	return 0;
}
