#include <stdio.h>

#include "tree.h"

int main(void){
	struct binary_tree *tree = init_tree();
	// TODO: handle case where it's one long chain of right nodes
	insert_key(tree, 2);
	insert_key(tree, 1);
	insert_key(tree, 3);
	insert_key(tree, 4);
	insert_key(tree, 5);
	insert_key(tree, 6);
	insert_key(tree, 7);
	insert_key(tree, 8);
	insert_key(tree, 9);
	insert_key(tree, 10);
	printf("Printing tree:\n");
	print_tree(tree->root);
	if(is_tree_balanced(tree) == 0){
		tree = balance_tree(tree);
	}
	printf("Printing tree:\n");
	print_tree(tree->root);
	is_tree_balanced(tree);
	free_tree(tree);
	return 0;
}
