#include "common.h"

struct p_data local_data;

void send_initial_part(double **a_temp, double **b_temp, int i, int j){
	int dest = j + (i*NUM_DIVISIONS);
	int shift = (i + j) % ((int) sqrt(NUM_PARTS));
	extract_part(A_MAT, a_temp, i*PART_SIZE, shift*PART_SIZE);
	extract_part(B_MAT, b_temp, shift*PART_SIZE, j*PART_SIZE);
	//print_initial_send(dest, i, j, shift, a_temp, b_temp);
	MPI_Send(*a_temp, PART_SIZE * PART_SIZE, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
	MPI_Send(*b_temp, PART_SIZE * PART_SIZE, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
}

// Sends the initial parts to the corresponding destination node.
void send_all_initial_parts(){
	double **a_temp = create_empty_matrix(PART_SIZE, PART_SIZE);
	double **b_temp = create_empty_matrix(PART_SIZE, PART_SIZE);
	int i, j;
	for(i = 0; i < NUM_DIVISIONS; i++){
		for(j = 0; j < NUM_DIVISIONS; j++){
			if(i == 0 && j == 0){
				continue;
			}
			send_initial_part(a_temp, b_temp, i, j);
		}
	}
	free_mat(a_temp);
	free_mat(b_temp);
}

// Only process with rank 0 will come here
void prepare_data(){
	send_all_initial_parts();
	local_data.i = 0;
	local_data.j = 0;
	extract_part(A_MAT, local_data.a_part, 0, 0);
	extract_part(B_MAT, local_data.b_part, 0, 0);
}

void receive_initial_parts(){
	MPI_Status s;
	MPI_Recv(local_data.a_recv_buf, PART_SIZE * PART_SIZE, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &s);
	MPI_Recv(local_data.b_recv_buf, PART_SIZE * PART_SIZE, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &s);
	memcpy(local_data.a_part_buf, local_data.a_recv_buf, PART_SIZE * PART_SIZE * sizeof(double));
	memcpy(local_data.b_part_buf, local_data.b_recv_buf, PART_SIZE * PART_SIZE * sizeof(double));
	set_matrix_from_buf(local_data.a_part, local_data.a_part_buf, PART_SIZE, PART_SIZE);
	set_matrix_from_buf(local_data.b_part, local_data.b_part_buf, PART_SIZE, PART_SIZE);
}

// Sets the nodes that this process will receive A and B from.
void set_a_b_source_nodes(){
	int a_source_i, a_source_j, b_source_i, b_source_j;
	a_source_i = local_data.i;
	if(local_data.j == NUM_DIVISIONS - 1){
		a_source_j = 0;
	} else {
		a_source_j = local_data.j + 1;
	}
	b_source_j = local_data.j;
	if(local_data.i == NUM_DIVISIONS - 1){
		b_source_i = 0;
	} else {
		b_source_i = local_data.i + 1;
	}
	local_data.a_source = a_source_j + (a_source_i * NUM_DIVISIONS);
	local_data.b_source = b_source_j + (b_source_i * NUM_DIVISIONS);
}

// Sets the nodes that this process's A and B copies will be sent to.
void set_a_b_dest_nodes(){
	int a_dest_i, a_dest_j, b_dest_i, b_dest_j;
	a_dest_i = local_data.i;
	b_dest_j = local_data.j;
	if(local_data.j == 0){
		a_dest_j = NUM_DIVISIONS - 1;
	} else {
		a_dest_j = local_data.j - 1;
	}
	if(local_data.i == 0){
		b_dest_i = NUM_DIVISIONS - 1;
	} else {
		b_dest_i = local_data.i - 1;
	}
	local_data.a_dest = a_dest_j + (a_dest_i * NUM_DIVISIONS);
	local_data.b_dest = b_dest_j + (b_dest_i * NUM_DIVISIONS);
}

void set_i_j_from_rank(){
	if(local_data.rank >= NUM_DIVISIONS){
		local_data.i = local_data.rank / NUM_DIVISIONS;
		local_data.j = local_data.rank - (NUM_DIVISIONS * local_data.i);
	} else {
		local_data.i = 0;
		local_data.j = local_data.rank;
	}
}

void create_buffers(){
	local_data.a_part_buf = malloc(sizeof(double) * PART_SIZE * PART_SIZE);
	local_data.b_part_buf = malloc(sizeof(double) * PART_SIZE * PART_SIZE);
	local_data.a_part = create_matrix_from_buf(local_data.a_part_buf, PART_SIZE, PART_SIZE);
	local_data.b_part = create_matrix_from_buf(local_data.b_part_buf, PART_SIZE, PART_SIZE);
	local_data.c_part = create_empty_matrix(PART_SIZE, PART_SIZE);
	local_data.a_recv_buf = malloc(sizeof(double) * PART_SIZE * PART_SIZE);
	local_data.b_recv_buf = malloc(sizeof(double) * PART_SIZE * PART_SIZE);
}

void init(){
	MAT_SIZE = 15;
	NUM_DIVISIONS = 3;
	NUM_PARTS = NUM_DIVISIONS * NUM_DIVISIONS;
	PART_SIZE = MAT_SIZE / NUM_DIVISIONS;
	MPI_Comm_rank(MPI_COMM_WORLD, &(local_data.rank));
	create_buffers();
	set_i_j_from_rank();
	set_a_b_dest_nodes();
	set_a_b_source_nodes();
}

void cleanup(){
	free_mat(local_data.a_part);
	free_mat(local_data.b_part);
	free_mat(local_data.c_part);
	free(local_data.a_recv_buf);
	free(local_data.b_recv_buf);
}

// Note: each process should have already multiplied its initial A and B parts before coming here.
// Send A and B parts to the correct processes, and also receives A and B from the correct processes.
// Multiplies the received A and B parts
void do_iteration(){
	MPI_Status s;
	MPI_Request req_1;
	MPI_Request req_2;
	MPI_Isend(local_data.a_part_buf, PART_SIZE * PART_SIZE, MPI_DOUBLE, local_data.a_dest, 4, MPI_COMM_WORLD, &req_1);
	MPI_Recv(local_data.a_recv_buf, PART_SIZE * PART_SIZE, MPI_DOUBLE, local_data.a_source, 4, MPI_COMM_WORLD, &s);
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Isend(local_data.b_part_buf, PART_SIZE * PART_SIZE, MPI_DOUBLE, local_data.b_dest, 5, MPI_COMM_WORLD, &req_2);
	MPI_Recv(local_data.b_recv_buf, PART_SIZE * PART_SIZE, MPI_DOUBLE, local_data.b_source, 5, MPI_COMM_WORLD, &s);
	MPI_Barrier(MPI_COMM_WORLD);
	memcpy(local_data.a_part_buf, local_data.a_recv_buf, PART_SIZE * PART_SIZE * sizeof(double));
	memcpy(local_data.b_part_buf, local_data.b_recv_buf, PART_SIZE * PART_SIZE * sizeof(double));
	MPI_Barrier(MPI_COMM_WORLD);
	mat_mul_part(local_data.a_part, local_data.b_part, local_data.c_part, PART_SIZE, PART_SIZE, PART_SIZE);
	MPI_Barrier(MPI_COMM_WORLD);
}

// Only process 0 should enter here.
// Receives partial results from all other processes and combines them.
void collect_final_result(){
	MPI_Status s;
	int i, j;
	//double *recv_buf = malloc(sizeof(double) * PART_SIZE * PART_SIZE);
	double **recv = create_empty_matrix(PART_SIZE, PART_SIZE);
	for(i = 0; i < NUM_DIVISIONS; i++){
		for(j = 0; j < NUM_DIVISIONS; j++){
			if(i == 0 && j == 0){
				insert_part(local_data.c_part, RESULT_MAT, 0, 0);
				continue; 
			}
			MPI_Barrier(MPI_COMM_WORLD);
			int recv_source = j + (i*NUM_DIVISIONS);
			MPI_Recv(*recv, PART_SIZE * PART_SIZE, MPI_DOUBLE, recv_source, 4, MPI_COMM_WORLD, &s);
			insert_part(recv, RESULT_MAT, i * PART_SIZE , j * PART_SIZE);
		}
	}
	free_mat(recv);
}

void send_final_result(){
	int i;
	for(i = 1; i < NUM_PARTS; i++){
		MPI_Barrier(MPI_COMM_WORLD);
		if(local_data.rank == i){
			MPI_Send(*local_data.c_part, PART_SIZE * PART_SIZE, MPI_DOUBLE, 0, 4, MPI_COMM_WORLD);
		}
	}
}

// TODO:
// investigate using transpose
// remove un-needed barriers
// further cleanup
// use ANY_TAG where possible
int main(int argc, char *argv[]){
	MPI_Init(&argc, &argv);
	init();
	if(local_data.rank == 0){
		create_initial_matrices();
		prepare_data();
	} else {
		receive_initial_parts();
	}
	mat_mul_part(local_data.a_part, local_data.b_part, local_data.c_part, PART_SIZE, PART_SIZE, PART_SIZE);
	MPI_Barrier(MPI_COMM_WORLD);
	int i;
	for(i = 1; i < NUM_DIVISIONS; i++){
		do_iteration();
	}
	if(local_data.rank == 0){
		collect_final_result();
		printf("Final C is:\n");
		print_matrix(RESULT_MAT, MAT_SIZE, MAT_SIZE);
		check_result_matches_control();
	} else {
		send_final_result();	
	}
	MPI_Finalize();
	return 0;
}

