#include "tree.h"

int main(void){
	struct binary_tree *tree = init_tree();
	insert_data(tree, 5);
	insert_data(tree, 2);
	insert_data(tree, 1);
	insert_data(tree, 7);
	insert_data(tree, 6);
	print_tree(tree->root);
	return 0;
}
