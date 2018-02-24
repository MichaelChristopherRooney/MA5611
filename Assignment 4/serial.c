#include <stdio.h>

#include "tree.h"

int main(void){
	struct binary_tree *tree = init_tree();
	insert_key(tree, 100);
	insert_key(tree, 50);
	insert_key(tree, 200);
	insert_key(tree, 25);
	insert_key(tree, 75);
	insert_key(tree, 85);
	insert_key(tree, 95);
	insert_key(tree, 15);
	insert_key(tree, 5);
	print_tree(tree);
	delete_key(tree, 50);
	print_tree(tree);
	/* TODO get this working
	insert_key(tree, 336465782);
	insert_key(tree, 278722862);
	insert_key(tree, 2145174067);
	print_tree(tree);
	delete_key(tree, 336465782);
	print_tree(tree);
	*/
	/*
	if(is_tree_balanced(tree) == 0){
		tree = balance_tree(tree);
	}
	print_tree(tree);
	is_tree_balanced(tree);
	*/
	free_tree(tree);
	return 0;
}
