#include <stdio.h>

#include "tree.h"

int main(void){
	struct binary_tree *tree = init_tree();
	insert_data(tree, 2);
	insert_data(tree, 1);
	insert_data(tree, 3);
	insert_data(tree, 4);
	insert_data(tree, 5);
	insert_data(tree, 6);
	//insert_data(tree, 5);
	//insert_data(tree, 2);
	//insert_data(tree, 1);
	//insert_data(tree, 7);
	//insert_data(tree, 6);
	//insert_data(tree, 8);
	printf("Printing tree:\n");
	print_tree(tree->root);
	is_tree_balanced(tree);
	//delete_data(tree, 7);
	//printf("Printing tree:\n");
	//print_tree(tree->root);
	return 0;
}
