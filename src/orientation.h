#ifndef ORIENT
#define ORIENT

struct Node {
	int v;			/* vertex identifier */
	struct Node *next;	/* pointer to next node in adjacency list*/
	int orientation;	/* -1, 0 or 1 depending on type of orientation */

};

struct Orientation {
	int n;				/* number of vertices */
	int m;				/* number of edges */	
	struct Node **adj_list;		/* list of vertices */
	int size_ef;			/* Size of the elimination front. */
	struct DynamicArray *ef;	// Elimination front. OBS: We will _always_ 
					// force the ef to be sorted in increasing
					// order.

	struct DynamicArray
		**reach_from;		/* For each element of the ef, from which
					   other elements is it reachable? */
	struct DynamicArray
		**reach_to;		/* For each element of the ef, elements
				   	to which it is reachable. */

	// reach_from[i] and reach_to[i] all have size "size_ef",
	// and reach_from[i][j] = -1 iff j is not an element of reach_from[i].
};

struct Node *create_node(int v);

struct Orientation *create_orientation (int no_v);

void append_node (struct Node **list, struct Node *node, int v);

void add_edge (struct Orientation *orientation, int src, int dest);

void print_orientation (struct Orientation *orientation);

void delete_edge (struct Orientation *orientation, int src, int dest);

void orient_edge (struct Orientation *orientation, int src, int dest);

bool delete_initial_node (struct Orientation *orientation, int v);

void delete_list (struct Orientation *orientation, int v);

void delete_orientation(struct Orientation *orientation);

bool is_reachable(struct Orientation *orientation, int src, int dest);

struct Orientation *create_complete_graph (int n);

struct Orientation *create_cycle (int n);

struct Orientation *create_complete_orientation (int n);

struct Orientation *import_from_file (char *fileName);

bool are_reach_relations_equal (struct Orientation *orient1, struct Orientation *orient2, int *set_of_vertices, int length);

int *compute_elimination_front(struct Orientation *orient, int *size_ef);

struct Orientation *copy_orientation (struct Orientation *original);

bool is_strongly_connected(struct Orientation *orient);

bool is_self_reachable (struct Orientation *orient, int i);

void compute_lex_order (int *u, int *v, struct Orientation *undir);

void test_edge_order();

void test_copy();

void test_reachability();

bool is_cyclic(struct Orientation *o, int v);

struct Orientation *create_erdos_renyi (int n, double p);

struct Orientation *get_reachability_orientation (struct Orientation *original, int* ef, int size_ef);

bool is_source_or_sink (struct Orientation *orient, int v);

#endif
