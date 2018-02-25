#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "tree.h"

static pthread_mutex_t MUTEX;
static pthread_t BALANCE_THREAD;
static pthread_t DELETE_THREAD;;
static pthread_t INSERT_THREAD;

static struct binary_tree *tree;

const static int BALANCE_MIN_SLEEP_TIME = 10;
const static int BALANCE_MAX_SLEEP_TIME = 20;
const static int DELETE_MIN_SLEEP_TIME = 3;
const static int DELETE_MAX_SLEEP_TIME = 10;
const static int INSERT_MIN_SLEEP_TIME = 1;
const static int INSERT_MAX_SLEEP_TIME = 5;

static void *balance(void *arg){
	while(1){
		sleep(rand() % (BALANCE_MAX_SLEEP_TIME + 1 - BALANCE_MIN_SLEEP_TIME) + BALANCE_MIN_SLEEP_TIME);
		pthread_mutex_lock(&MUTEX);
		if(is_tree_balanced(tree) == 0){
			printf("Tree is not balanced, balancing now...\n");
			tree = balance_tree(tree);
			print_tree(tree);
		} else {
			printf("Tree does not need to be balanced\n");
		}
		pthread_mutex_unlock(&MUTEX);
	}
	return NULL;
}

static void *delete(void *arg){
	while(1){
		sleep(rand() % (DELETE_MAX_SLEEP_TIME + 1 - DELETE_MIN_SLEEP_TIME) + DELETE_MIN_SLEEP_TIME);
		pthread_mutex_lock(&MUTEX);
		if(tree->root == NULL){
			printf("Tree is empty, nothing to delete\n");
		} else {
			int key = get_random_key_from_tree(tree);
			printf("Got random key %d\n", key);
			delete_key(tree, key);
			printf("Deleted %d from tree\n", key);
			print_tree(tree);
		}
		pthread_mutex_unlock(&MUTEX);
	}
	return NULL;
}

// Picks a random number using rand()'s full range
// This makes it unlikely there will ever be a collision.
// If there is a collision the tree code will fail to work though.
static void *inserte(void *arg){
	while(1){
		sleep(rand() % (INSERT_MAX_SLEEP_TIME + 1 - INSERT_MIN_SLEEP_TIME) + INSERT_MIN_SLEEP_TIME);
		pthread_mutex_lock(&MUTEX);
		int key = rand();
		insert_key(tree, key);
		printf("Inserted %d\n", key);
		print_tree(tree);
		pthread_mutex_unlock(&MUTEX);
	}
	return NULL;
}

int main(void){
	tree = init_tree();
	pthread_create(&BALANCE_THREAD, NULL, balance, NULL);
	pthread_create(&DELETE_THREAD, NULL, delete, NULL);
	pthread_create(&INSERT_THREAD, NULL, inserte, NULL);
	while(1){}
	return 0;
}
