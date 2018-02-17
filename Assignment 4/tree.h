struct node {
	int key;
	struct node *left;
	struct node *right;
	struct node *parent;
};

struct binary_tree {
	struct node *root;
	int total_size;
};

struct binary_tree *init_tree();
void free_tree(struct binary_tree *tree);
void insert_key(struct binary_tree *tree, int key);
void print_tree(struct node *cur);
void delete_key(struct binary_tree *tree, int key);
int is_tree_balanced(struct binary_tree *tree);
struct binary_tree *balance_tree(struct binary_tree *tree);
