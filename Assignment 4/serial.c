#include <stdio.h>

#include "tree.h"

int main(void){
	struct binary_tree *tree = init_tree();
	insert_key(tree, 2);
	insert_key(tree, 1);
	insert_key(tree, 3);
	insert_key(tree, 4);
	insert_key(tree, 5);
	insert_key(tree, 6);
	printf("Printing tree:\n");
	print_tree(tree->root);
	is_tree_balanced(tree);
	free_tree(tree);
	return 0;
}
