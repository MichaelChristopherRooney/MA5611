#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "tree.h"

static pthread_mutex_t mutex;

static pthread_t balancer_thread;
static pthread_t deleter_thread;
static pthread_t inserter_thread;

static struct binary_tree *tree;

static void *balancer(void *arg){
	printf("Balanced\n");
	while(1){
		sleep(rand() % 10);
		printf("Balancer trying to aquire mutex\n");
		pthread_mutex_lock(&mutex);
		printf("Balancer aquired mutex\n");
		//sleep(3);
		printf("Balancer releasing mutex\n");
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

static void *deleter(void *arg){
	printf("Deleter\n");
	while(1){
		sleep(rand() % 10);
		printf("Deleter trying to aquire mutex\n");
		pthread_mutex_lock(&mutex);
		printf("Deleter aquired mutex\n");
		if(tree->root == NULL){
			printf("Tree is empty, nothing to delete\n");
		} else {
			int key = get_random_key_from_tree(tree);
			printf("Got random key %d\n", key);
			delete_key(tree, key);
			printf("Deleted %d from tree\n", key);
			print_tree(tree);
		}
		printf("Deleter releasing mutex\n");
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

// Picks a random number using rand()'s full range
// This makes it unlikely there will ever be a collision.
// If there is a collision the tree code will fail to work though.
static void *inserter(void *arg){
	printf("Inserter\n");
	while(1){
		sleep(rand() % 10);
		printf("Inserter trying to aquire mutex\n");
		pthread_mutex_lock(&mutex);
		printf("Inserter aquired mutex\n");
		int key = rand();
		insert_key(tree, key);
		printf("Inserted %d\n", key);
		print_tree(tree);
		printf("Inserter releasing mutex\n");
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

int main(void){
	tree = init_tree();
	pthread_create(&balancer_thread, NULL, balancer, NULL);
	pthread_create(&deleter_thread, NULL, deleter, NULL);
	pthread_create(&inserter_thread, NULL, inserter, NULL);
	while(1){}
	return 0;
}
