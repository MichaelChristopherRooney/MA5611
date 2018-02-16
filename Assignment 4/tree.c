#include <stdlib.h>
#include <stdio.h>

#include "tree.h"

struct binary_tree *init_tree(){
	struct binary_tree *b = calloc(1, sizeof(struct binary_tree));
	return b;
}

void insert_data(struct binary_tree *tree, int data){
	if(tree->root == NULL){
		tree->root = calloc(1, sizeof(struct node));
		tree->root->data = data;
		return;
	}
	struct node *cur = tree->root;
	while(1){
		if(data > cur->data){
			if(cur->right == NULL){
				struct node *r = calloc(1, sizeof(struct node));
				r->data = data;
				r->parent = cur;
				cur->right = r;
				return;
			} else {
				cur = cur->right;
			}
		} else {
			if(cur->left == NULL){
				struct node *l = calloc(1, sizeof(struct node));
				l->data = data;
				l->parent = cur;
				cur->left = l;
				return;
			} else {
				cur = cur->left;
			}
		}
	}
}

// Prints tree left to right
// Pass root node to print entire tree
void print_tree(struct node *cur){
	if(cur->left != NULL){
		print_tree(cur->left);
	}
	printf("%d\n", cur->data);
	if(cur->right != NULL){
		print_tree(cur->right);
	}
}
