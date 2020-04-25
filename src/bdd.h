#ifndef BDDGEN
#define BDDGEN

struct BDDNode {
	int v;			/* index of variable */
	struct BDDNode *lo;	/* pointer to node when false */
	struct BDDNode *hi;	/* pointer to node when true */
	bool is_visited;   	// get rid of this later
};

struct TONode {
	// Node for the array representing a topological order
	// of a BDD.
	//struct BDDNode *bdd;
	int v;
	int lo;	
	int hi;	
};

struct TOStack {
	struct TONode *node_array;
	int n;				// number of elements present
	int l;				// length of array
};

struct BDDNode *new_null_bdd_node (int v);

void del_one_bdd_node (struct BDDNode *node);

int size_of_bdd (struct BDDNode *node);

void delete_bdd (struct BDDNode *node);

struct TOStack *contruct_to_stack (struct BDDNode *bdd);

int *compute_profile(struct TOStack *s, int *length);

void test_stack(struct BDDNode *bdd);
#endif
