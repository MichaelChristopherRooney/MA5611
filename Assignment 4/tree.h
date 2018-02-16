struct node {
	int data;
	struct node *left;
	struct node *right;
	struct node *parent;
};

struct binary_tree {
	struct node *root;
	int max_depth; // TODO
	int total_size; // TODO
};

struct binary_tree *init_tree();
void insert_data(struct binary_tree *tree, int data);
void print_tree(struct node *cur);
void delete_data(struct binary_tree *tree, int data);
