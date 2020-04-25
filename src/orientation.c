#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

#include "orientation.h"
#include "dynamic-array.h"
#include "general-utils.h"

struct Node *create_node(int v) {
	struct Node *new_node = (struct Node *) malloc(sizeof(struct Node));
	new_node -> v = v;
	new_node -> next = NULL;
	new_node -> orientation = 0;
	return new_node;
}


struct Orientation *create_orientation (int noV) {
	struct Orientation *orientation = malloc(sizeof(struct Orientation));
	orientation -> n = noV;
	orientation -> m = 0;
	orientation -> adj_list = malloc(noV * sizeof(struct Node *));
	for (int i = 0; i < noV; i++) {
		orientation -> adj_list[i] = NULL;
	}
	orientation -> size_ef = -1;
	orientation -> reach_to  = NULL;
	orientation -> reach_from = NULL;
	orientation -> ef = NULL;
	return orientation;
}


void append_node (struct Node **list, struct Node *node, int v) {
	/* list: adjacency list of the orientation */
	/* node: node to be added*/
	/* v: index for the list*/

	if (list[v] == NULL) {
		list[v] = node;
		return;
	}
	
	struct Node *temp;
	temp = list[v];
	
	while(temp -> next != NULL) {
		temp = temp -> next;
	}

	if (temp -> next == NULL) {
		temp -> next = node;
	} else {
		printf("Something bad happened");
	}
}

void add_edge (struct Orientation *orientation, int src, int dest) {
	struct Node *new_node = create_node(dest);
	append_node(orientation -> adj_list, new_node, src);
	new_node = create_node(src);
	append_node(orientation -> adj_list, new_node, dest);
	(orientation -> m)++;
}

void print_orientation (struct Orientation *orientation) {
	int v;
	struct Node *temp;
	printf("------\n");
	for (v = 0; v < orientation -> n; v++) {
		temp = orientation -> adj_list[v];
		printf("\n Adjacency list of vertex %d\n ", v);
		while (temp != NULL) {
			printf("%d ", temp -> v);
			printf("(%d)", temp -> orientation);
			printf(" -> ");
			temp = temp -> next;
		}
		printf("\n");
	}
	if(orientation->size_ef > 0) printf("ef: ");
	da_print(orientation->ef);
	if(orientation->size_ef > 0) printf("\n");
	for(int i = 0; i < orientation->size_ef; i++) {
		printf("reach_from");
		printf("[%d] (ef[%d] = %d): ", i, i, da_get(i, orientation->ef));
		da_print(orientation->reach_from[i]);
		printf("\n");
	}
	printf("\n");
	for(int i = 0; i < orientation->size_ef; i++) {
	printf("reach_to  ");
		printf("[%d] (ef[%d] = %d): ", i, i, da_get(i, orientation->ef));
		da_print(orientation->reach_to[i]);
		printf("\n");
	}

	printf("------\n");
}

void delete_edge_aux (struct Orientation *orientation, int src, int dest) {
	/* deletes Node dest from list of src*/
	/* Attention: must run twice (both directions) for undirected*/
	struct Node *node, *next, *before;

	if (orientation -> adj_list[src] == NULL) {
		printf("error, the node isn't in the list\n");
		return;
	}


	before = orientation -> adj_list[src];
	if (before -> v == dest) {
		orientation -> adj_list[src] = before -> next;
		free(before);
		return;
	}
	
	while ((before -> next) -> v != dest) {
		before = before -> next;
	}

	node = before -> next;
	next = node -> next;

	before -> next = next;
	free(node);
}

void delete_edge (struct Orientation *orientation, int src, int dest) {
	/* deletes Node dest from list of src*/
	delete_edge_aux(orientation, src, dest);
	delete_edge_aux(orientation, dest, src);
}

bool is_adjacent_to_directed_edge(struct Orientation *orient, int v) {
	// Returns true iff v is adjacent to some oriented edge.
	struct Node *p = orient->adj_list[v]; 
	while(p != NULL) {
		if(p->orientation == 1 || p->orientation == -1) {
			return true;
		}
		p = p->next;
	}
	return false;
}

bool is_adjacent_to_undirected_edge(struct Orientation *orient, int v) {
	// Returns true iff v is adjacent to some undirected edge.
	struct Node *p = orient->adj_list[v]; 
	while(p != NULL) {
		if(p->orientation == 0) {
			return true;
		}
		p = p->next;
	}
	return false;
}

void orient_edge (struct Orientation *orientation, int src, int dest) { /* orient from src to dest*/ struct Node *node = orientation -> adj_list[src];
	int size_ef_original = orientation->size_ef;
	bool is_src_in_ef_in_original;
	bool is_dest_in_ef_in_original;

	if(is_adjacent_to_directed_edge(orientation, src) &&
			is_adjacent_to_undirected_edge(orientation, src)) {
		is_src_in_ef_in_original = true;
	} else {
		is_src_in_ef_in_original = false;
	}

	if(is_adjacent_to_directed_edge(orientation, dest) &&
			is_adjacent_to_undirected_edge(orientation, dest)) {
		is_dest_in_ef_in_original = true;
	} else {
		is_dest_in_ef_in_original = false;
	}

	while(node -> v != dest) 
		node = node -> next;

	node -> orientation = 1;


	node = orientation -> adj_list[dest];
	while(node -> v != src)
		node = node -> next;
	node -> orientation = -1;

	// From here on, we refresh information about ef.
	
	if(orientation->reach_from == NULL) {
		// This is the first edge to be oriented.
		orientation -> size_ef = 2;
		orientation -> ef = da_initialize();
		if(src < dest) {
			da_append(src, orientation->ef);
			da_append(dest, orientation->ef);
		} else {
			da_append(dest, orientation->ef);
			da_append(src, orientation->ef);
		}

		assert(orientation->size_ef == da_size(orientation->ef));
		orientation -> reach_from = 
			malloc(orientation -> size_ef * sizeof(struct DynamicArray *));
		orientation -> reach_to = 
			malloc(orientation -> size_ef * sizeof(struct DynamicArray *));

		for(int i = 0; i < orientation->size_ef; i++) {
			orientation -> reach_from[i] = da_initialize();
			orientation -> reach_to[i] = da_initialize();
		}
		int ef_idx_src = da_get_idx(src, orientation->ef);
		int ef_idx_dest = da_get_idx(dest, orientation->ef);
		assert(ef_idx_src != -1 && ef_idx_dest != -1);

		da_append(src, orientation->reach_from[ef_idx_dest]);
		da_append(dest, orientation->reach_to[ef_idx_src]);
		return;
	}

	// This is NOT the first edge to be oriented.
	// Remember: the only elements whose "is in" relation with respect
	// to the ef can change are src and dest;
	
	// Define the following variables (with respect to new ef)
	bool is_src_in_ef;
	bool is_dest_in_ef;

	if(is_adjacent_to_directed_edge(orientation, src) &&
			is_adjacent_to_undirected_edge(orientation, src)) {
		is_src_in_ef = true;
	} else {
		is_src_in_ef = false;
	}

	if(is_adjacent_to_directed_edge(orientation, dest) &&
			is_adjacent_to_undirected_edge(orientation, dest)) {
		is_dest_in_ef = true;
	} else {
		is_dest_in_ef = false;
	}

	// Update elimination front.
	bool is_there_change[4]; 	// Is there any change in ef?
	for(int i = 0; i < 4; i++) is_there_change[i] = false;
	if(is_src_in_ef && !is_src_in_ef_in_original) {
		orientation->size_ef++;
		is_there_change[0] = true;
	}
	if(!is_src_in_ef && is_src_in_ef_in_original) {
		orientation->size_ef--;
		is_there_change[1] = true;
	}
	if(is_dest_in_ef && !is_dest_in_ef_in_original) {
		orientation->size_ef++;
		is_there_change[2] = true;
	}
	if(!is_dest_in_ef && is_dest_in_ef_in_original) {
		orientation->size_ef--;
		is_there_change[3] = true;
	}
	bool was_there_change_in_ef = (is_there_change[0] || 
			is_there_change[1] || is_there_change[2] || is_there_change[3]);

	struct DynamicArray* prev_ef = orientation->ef;

	if(!was_there_change_in_ef) {
		// There was no change in the ef, so we don't need
		// to update it.
		goto jump;
	}

	orientation->ef = da_copy(prev_ef);

	// The only memberships of ef that can possibly change
	// are src and dest, and we can decide which of them have changed
	// it using the variables as defined above.
	
	if(is_src_in_ef && !is_src_in_ef_in_original) {
		da_add_sorted(src, orientation->ef);
	}
	if(!is_src_in_ef && is_src_in_ef_in_original) {
		da_remove_element(src, orientation->ef);
	}
	if(is_dest_in_ef && !is_dest_in_ef_in_original) {
		da_add_sorted(dest, orientation->ef);
	}
	if(!is_dest_in_ef && is_dest_in_ef_in_original) {
		da_remove_element(dest, orientation->ef);
	}


jump:;
	// Update reach_from and reach_to for src and dest.
	//
	// First, we allocate the memory for the new ones.
	assert(orientation->size_ef == da_size(orientation->ef));
	struct DynamicArray **original_reach_from = orientation->reach_from;
	struct DynamicArray **originalReachTo = orientation->reach_to;
	orientation->reach_from = malloc(orientation->size_ef * 
			sizeof(struct DynamicArray*));
	orientation->reach_to = malloc(orientation->size_ef * 
			sizeof(struct DynamicArray*));


	for(int i = 0; i < orientation->size_ef; i++) {
		orientation->reach_from[i] = da_initialize();
		orientation->reach_to[i] = da_initialize();
	}

	// From here on, we update reach_from and reach_to	
	int src_idx, dest_idx, p_src_idx, p_dest_idx;		// p = previous
	src_idx = da_get_idx(src, orientation->ef);	
	dest_idx = da_get_idx(dest, orientation->ef);	
	p_src_idx = da_get_idx(src, prev_ef);	
	p_dest_idx = da_get_idx(dest, prev_ef);
	
	if(is_src_in_ef && is_src_in_ef_in_original) {
		orientation->reach_from[src_idx] = da_copy(original_reach_from[p_src_idx]);
	}


	if(!is_src_in_ef) {
		;	
	} else if (is_src_in_ef_in_original && is_dest_in_ef_in_original) {
		orientation->reach_to[src_idx] = da_union(originalReachTo[p_src_idx], 
			originalReachTo[p_dest_idx], true);
		da_add_sorted(dest,orientation->reach_to[src_idx]);
	} else if(is_src_in_ef_in_original) {
		orientation->reach_to[src_idx] = da_copy(originalReachTo[p_src_idx]);
		da_add_sorted(dest,orientation->reach_to[src_idx]);
	} else if(is_dest_in_ef_in_original) {
		orientation->reach_to[src_idx] = da_copy(originalReachTo[p_dest_idx]);
		da_add_sorted(dest,orientation->reach_to[src_idx]);
	}

	if(!is_dest_in_ef) {
		;	
	} else if (is_src_in_ef_in_original && is_dest_in_ef_in_original) {
		orientation->reach_from[dest_idx] = da_union(original_reach_from[p_src_idx], 
			original_reach_from[p_dest_idx], true);
		da_add_sorted(src,orientation->reach_from[dest_idx]);
	} else if(is_src_in_ef_in_original) {
		orientation->reach_from[dest_idx] = da_copy(original_reach_from[p_src_idx]);
		da_add_sorted(src,orientation->reach_from[dest_idx]);
	} else if(is_dest_in_ef_in_original) {
		orientation->reach_from[dest_idx] = da_copy(original_reach_from[p_dest_idx]);
		da_add_sorted(src,orientation->reach_from[dest_idx]);
	}


	if(is_dest_in_ef && is_dest_in_ef_in_original) {
		orientation->reach_to[dest_idx] = da_copy(originalReachTo[p_dest_idx]);
	}

	assert(orientation->size_ef == da_size(orientation->ef));
	
	for(int idx = 0; idx < orientation->size_ef; idx++) {
		int v = da_get(idx, orientation->ef);
		if (idx == src_idx || idx == dest_idx) {
			continue;
		}
		int p_idx = da_get_idx(v, prev_ef);
		assert(p_idx != -1);
		orientation->reach_from[idx] = da_copy(original_reach_from[p_idx]);
		if(da_does_contain(dest, orientation->reach_from[idx], true)) {
			da_add_sorted(src, orientation->reach_from[idx]);
		}
		orientation->reach_to[idx] = da_copy(originalReachTo[p_idx]);
		if(da_does_contain(src, orientation->reach_to[idx], true)) {
			da_add_sorted(dest, orientation->reach_to[idx]);
		}
	}


	// Free up memory of previous ef, reach_from and reach_to.
	if(was_there_change_in_ef) {
		da_delete(prev_ef);
	}
	for(int i = 0; i < size_ef_original; i++) {
		if(original_reach_from[i] != NULL)
			da_delete(original_reach_from[i]);
		if(originalReachTo[i] != NULL)
			da_delete(originalReachTo[i]);
	}
	if(original_reach_from != NULL) {
		free(original_reach_from);
	}
	if(originalReachTo != NULL) {
		free(originalReachTo);
	}
}

bool delete_initial_node (struct Orientation *orientation, int v) {
	/* Returns false iff adj_list[v] is empty. */

	struct Node *initial = orientation -> adj_list[v];
	if (initial == NULL)
		return false;
	orientation -> adj_list[v] = initial -> next;
	free(initial);
	return true;
}

void delete_list (struct Orientation *orientation, int v) {
	// Deletes adj_list[v].
	// (OBS: for now, use only when deleting the orientation!)
	
	while(delete_initial_node(orientation, v))
		;
}

void delete_orientation(struct Orientation *orientation) {
	// Frees memory used by orientation.
	
	int n = orientation -> n;
	for (int i = 0; i < n; i++) {
		delete_list(orientation, i);
	}
	free (orientation -> adj_list);
	free(orientation->ef);
	free(orientation->reach_to);
	free(orientation->reach_from);
	free(orientation);
}

bool is_reachable_aux(struct Orientation *orientation, int src, int dest, bool *visited) {
	// Auxiliary function for is_reachable function.
	
	if (visited[src]) {
		/*printf("already visited\n"); */
		return false;
	}
	visited[src] = true;
	struct Node *initial = orientation -> adj_list[src];
	if (initial == NULL) {
		/*printf("initial was null\n");*/
		return false;
	}
	bool result = false;
	while (initial != NULL) {
		/*printf("until here?\n");*/
		if ((initial -> v == dest) && (initial -> orientation == 1))
			return true;
		if ((initial -> orientation == 1) && is_reachable_aux(orientation, initial -> v, dest, visited))
			result = true;
		initial = initial -> next;
	}
	return result;
}

bool is_reachable(struct Orientation *orientation, int src, int dest) {
	/* Returns true iff dest is reachable from src
	 * in partial orientation.
	 * Assumes that src is different from dest. */
	int n = orientation -> n;
	bool *visited;
	visited = (bool *) malloc(n * sizeof(bool));
	for (int i = 0; i < n; i++)
		visited[i] = false;
	bool result = is_reachable_aux(orientation, src, dest, visited);
	free(visited);
	return result;
}

struct Orientation *create_complete_graph (int n) {
	// Allocates memory and returns pointer to a complete
	// undirected graph on n vertices.
	struct Orientation *graph = create_orientation(n);
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			if (i < j)
				add_edge(graph, i, j);
	return graph;
}

struct Orientation *create_cycle (int n) {
	// Allocates memory and returns pointer to a cycle
	// undirected graph on n vertices.
	struct Orientation *graph = create_orientation(n);
	for (int i = 0; i < n-1; i++)
		add_edge(graph, i, i+1);
	add_edge(graph, n-1, 0);
	return graph;
}

struct Orientation *create_complete_orientation (int n) {
	// Allocates memory and returns pointer to a completely
	// oriented complete graph on n vertices.
	struct Orientation *orientation = create_complete_graph(n);
	for (int i = 0; i < n; i++)
		for(int j = 0; j < n; j++)
			if (i < j) 
				orient_edge(orientation, i, j);
	return orientation;

}

bool are_reach_relations_equal (struct Orientation *orient1, struct Orientation *orient2, int* setOfVertices, int length) {
	/* this function assume that the set of oriented edges are the same*/
	/* (and thus that the elimination front is the same)*/
	/* it checks if the reachability relation on the set of vertices given is the same*/
	for(int i = 0; i < length; i++) {
		assert(da_get(i,orient1->ef) == da_get(i, orient2->ef));
		assert(da_is_sorted(orient1->reach_from[i]));
		assert(da_is_sorted(orient1->reach_to[i]));
		if(!da_are_elements_equal_in_order_restricted(orient1->reach_from[i], orient2->reach_from[i], orient1->ef)) {
			return false;
		}
		if(!da_are_elements_equal_in_order_restricted(orient1->reach_to[i], orient2->reach_to[i], orient1->ef)) {
			return false;
		}
	}

	return true;
}

bool is_vertex_in_ef (struct Orientation *orient, int i) {
	/* i: vertex under cosideration */
	bool is_there_und = false, is_there_dir = false;
	struct Node *p = orient -> adj_list[i];
	if (p == NULL) {
		return false;
	}
	while (p != NULL) {
		if((p -> orientation == 1) || (p -> orientation == -1)) {
			is_there_dir = true;
		}
		else {
			is_there_und = true;
		}
		p = p -> next;	
	}
	return is_there_und && is_there_dir;
}

int *compute_elimination_front(struct Orientation *orient, int *size_ef) {
	int n = orient -> n;
	int *ret_array;
	*size_ef = 0;
	int i;
	for (i = 0; i < n; i++) {
		if(is_vertex_in_ef(orient, i)) {
			(*size_ef)++;
		}
	}

	if((*size_ef) == 0) {
		printf("the size of the elimination front is 0\n");
		return NULL;
	}

	ret_array = (int *) malloc(sizeof(int) * (*size_ef));

	if(ret_array == NULL) {
		printf("BAD\n");
		return NULL;
	}
	
	int j = 0;
	for (i = 0; i < n; i++) {
		if(is_vertex_in_ef(orient, i)) {
			ret_array[j++] = i;
		}
	}

	return ret_array;
}

struct Node *copy_node_list(struct Node *original) {
	struct Node *ret_val = NULL; 
	struct Node *trav = NULL;

	if (original == NULL) {
		return NULL;
	}

	ret_val = create_node(original -> v);
	ret_val -> orientation = original -> orientation;
	trav = ret_val;

	while ( original -> next != NULL ) {
		original = original -> next;
		trav -> next = create_node(original -> v);
		trav = trav -> next;
		trav -> orientation = original -> orientation;
	}

	return ret_val;
}

struct Orientation *copy_orientation (struct Orientation *original) {
	int i;
	struct Orientation *ret_val = create_orientation(original -> n);
	ret_val -> m = original -> m;
	for (i = 0; i < original -> n; i++) {
		ret_val -> adj_list[i] = copy_node_list(original -> adj_list[i]);
	}

	// ef, reach_from, reachTo
	
	ret_val->size_ef = original->size_ef;
	if(original->size_ef < 0) {
		ret_val->ef = NULL;
		ret_val->reach_from = NULL;
		ret_val->reach_to = NULL;
		return ret_val;
	}

	ret_val->ef = da_copy(original->ef);
	
	ret_val->reach_from = malloc(ret_val->size_ef * sizeof(struct DynamicArray*));
	ret_val->reach_to = malloc(ret_val->size_ef * sizeof(struct DynamicArray*));
	for(int i = 0; i < ret_val->size_ef; i++) {
		ret_val->reach_from[i] = da_copy(original->reach_from[i]);;	
		ret_val->reach_to[i] = da_copy(original->reach_to[i]);;	
	}

	return ret_val;
}


bool is_self_reachable_aux (struct Orientation *orient, int src, int dest, int i, bool *visited) {
	if (visited[src] && src != i) {
		/*printf("already visited\n"); */
		return false;
	}
	visited[src] = true;
	struct Node *initial = orient -> adj_list[src];
	if (initial == NULL) {
		/*printf("initial was null\n");*/
		return false;
	}
	bool result = false;
	while (initial != NULL) {
		/*printf("until here?\n");*/
		if ((initial -> v == dest) && (initial -> orientation == 1))
			return true;
		if ((initial -> orientation == 1) && is_self_reachable_aux(orient, initial -> v, dest, i, visited))
			result = true;
		initial = initial -> next;
	}
	return result;
}

bool is_self_reachable (struct Orientation *orient, int i) {
	// Assumes that orient is a digraph.
	// Returns true if and only if vertex i can reach itself.
	int n = orient -> n;
	bool *visited;
	visited = (bool *) malloc(n * sizeof(bool));
	for (int i = 0; i < n; i++)
		visited[i] = false;
	bool result = is_self_reachable_aux(orient, i, i, i, visited);
	free(visited);
	return result;
}

void compute_lex_order (int *u, int *v, struct Orientation *undir) {
	// This function receives an undirected (connected) graph, and
	// returns the lexicographical order of the edges in the form
	// of two int arrays u and v (i.e. #i-th edge = (u[i],v[i])).
	// It assumes memory has already been allocated to u and v.
	struct Orientation *o = copy_orientation(undir);
	int n = o -> n;
	int counter = 0;
	int next = -1;
	bool isFirst;
	struct Node *p;
	for(int i = 0; i < n; i++) {
		isFirst = true;
		if(o ->adj_list[i] == NULL) {
			printf("Error! Graph is not connected!");
			return;
		}
		p = o -> adj_list[i];
		while(p != NULL) {
			if(isFirst && p->orientation == 0) {
				isFirst = false;
				next = p->v;
			}
			if(p->orientation==0) {
				//print_orientation(o);
				orient_edge(o, i, p->v);
				u[counter] = i;
				v[counter++] = p->v;
			}
			p = p -> next;
		}
	}
	delete_orientation(o);
}

bool is_cyclic(struct Orientation *o, int v) {
	// This function is given a partial orientation o,
	// a vertex adjacent to the edge that has just been
	// oriented, and returns whether there is a cycle
	// containing v (i.e. whether the orientation is cyclic
	// if we use it throughout the whole BDD construction
	// process).
	if(o->adj_list[v] == NULL)
		return false;
	return is_self_reachable(o, v);	
}

struct Orientation *create_erdos_renyi (int n, double p) {
	struct Orientation *orientation = malloc(sizeof(struct Orientation));
	float r;

	orientation -> n = n;
	orientation -> adj_list = malloc(n * sizeof(struct Node *));
	for (int i = 0; i < n; i++) {
		orientation -> adj_list[i] = NULL;
	}
	
	for (int i = 0; i < n; i++) {
		for (int j = i+1; j < n; j++) {
			r = (rand()/(double)RAND_MAX);
			if( r < p ) {
				add_edge(orientation, i, j);
			}
		}
	}
	return orientation;
}

void test_edge_order() {
	struct Orientation *o = create_complete_graph(6);
	int m = o->m;
	int *u = malloc(m * sizeof(int));
	int *v = malloc(m * sizeof(int));
	compute_lex_order(u, v, o);
	for(int i = 0; i < m; i++) {
		printf("(u[%d],v[%d]) = (%d, %d)\n", i, i, u[i], v[i]);
	}

	free(u);
	free(v);
}

void test_copy() {
	struct Orientation *orient = NULL;
	struct Orientation *orient_copy = NULL;
	orient = create_cycle(5);
	orient_edge(orient, 2, 1);
	orient_edge(orient, 3, 2);
	orient_edge(orient, 3, 4);
	orient_copy = copy_orientation(orient);
	print_orientation(orient);
	print_orientation(orient_copy);
}

void test_reachability() {

	/* simple test 1*/
	struct Orientation *orient1 = create_complete_graph(4);
	struct Orientation *orient2 = create_complete_graph(4);
	delete_edge(orient1, 0, 3);
	delete_edge(orient1, 1, 2);
	delete_edge(orient2, 0, 3);
	delete_edge(orient2, 1, 2);

	orient_edge(orient1, 0, 1);
	orient_edge(orient1, 0, 2);
	orient_edge(orient2, 1, 0);
	orient_edge(orient2, 2, 0);

	int size_ef;
	int *eliminationFront = compute_elimination_front(orient1, &size_ef);

	if(are_reach_relations_equal(orient1, orient2, eliminationFront, size_ef)){
		printf("seems alrighty\n");
	}
	else 
		printf("bad\n");
	delete_orientation(orient1);
	delete_orientation(orient2);
	free(eliminationFront);

	/* simple test2*/
	orient1 = create_complete_graph(4);
	orient2 = create_complete_graph(4);
	delete_edge(orient1, 0, 3);
	delete_edge(orient1, 1, 2);
	delete_edge(orient2, 0, 3);
	delete_edge(orient2, 1, 2);

	orient_edge(orient1, 0, 1);
	orient_edge(orient1, 2, 0);
	orient_edge(orient1, 3, 1);
	orient_edge(orient2, 0, 2);
	orient_edge(orient2, 1, 0);
	orient_edge(orient2, 1, 3);

	eliminationFront = compute_elimination_front(orient1, &size_ef);
	if(are_reach_relations_equal(orient1, orient2, eliminationFront, size_ef)){
		printf("seems alrighty\n");
	}
	else 
		printf("bad\n");
	delete_orientation(orient1);
	delete_orientation(orient2);
	free(eliminationFront);
	
	/* simple test3: this time the ReachRelations are different*/
	orient1 = create_complete_graph(4);
	orient2 = create_complete_graph(4);
	delete_edge(orient1, 0, 3);
	delete_edge(orient1, 1, 2);
	delete_edge(orient2, 0, 3);
	delete_edge(orient2, 1, 2);

	orient_edge(orient1, 0, 1);
	orient_edge(orient1, 2, 0);
	orient_edge(orient1, 3, 1);

	orient_edge(orient2, 2, 0);
	orient_edge(orient2, 0, 1);
	orient_edge(orient2, 1, 3);
	eliminationFront = compute_elimination_front(orient1, &size_ef);
	if(are_reach_relations_equal(orient1, orient2, eliminationFront, size_ef)){
		printf("bad\n");
	}
	else 
		printf("seems alrighty\n");
	delete_orientation(orient1);
	delete_orientation(orient2);
	free(eliminationFront);
}

struct Orientation *import_from_file (char *fileName) {
	FILE *p;
	char line[256];
	int n = -1; // number of vertices
	int i, j;
	p = fopen(fileName, "r");
	if(p == NULL) {
		printf("File does not exist.\n");
		exit(1);
	}
	// First traversal, get number of nodes.
	while (fgets(line, sizeof(line), p)) {
		sscanf(line, "%d %d {}", &i, &j);
		if (i + 1 > n) {
			n = i+1;
		}
		if (j + 1 > n) {
			n = j+1;
		}
	}
	rewind(p);
	struct Orientation *ret_val = create_orientation(n);
	// Second traversal, add edges.
	while (fgets(line, sizeof(line), p)) {
		sscanf(line, "%d %d {}", &i, &j);
		add_edge(ret_val, i, j);
	}
	fclose(p);
	return ret_val;
}

void delete_oriented_edges (struct Orientation *orientation, int v) {
	/* returns false iff list is empty */
	struct Node *p = orientation -> adj_list[v]; //pointer
	struct Node *d = p; // to be deleted
	if (p == NULL) {
		return;
	}
	if(p -> next == NULL) {
		orientation -> adj_list[v] = p -> next;
		free(p);
		return;
	}

	// Initial nodes.
	while(p -> orientation == -1 || p->orientation == 1) {
		d = p;
		orientation -> adj_list[v] = p -> next;
		p = p -> next;
		free(d);
		if(p == NULL) {
			return;
		}
	}

	// Intermediate nodes.
	while(p -> next != NULL) {
		if(p -> next -> orientation == -1 || p->next->orientation == 1) {
			d = p->next;
			p->next = p->next->next;
			free(d);

		} else { // do not delete
			p = p -> next;
		}
	}
	
}

struct Orientation *get_reachability_orientation (struct Orientation *original, int* ef, int size_ef) {
	// Returns orientation where oriented edges have been substituted
	// for one oriented edge representing each related pair of vertices
	// in the elimination front
	int i, j;
	struct Orientation *ret_val = copy_orientation(original);
	//return ret_val;
	//printf("before (size ef) = %d\n", size_ef);
	//print_orientation(ret_val);
	for (i = 0; i < original -> n; i++) {
		delete_oriented_edges(ret_val, i);
	}

	for (i = 0; i < size_ef; i++) {
		for(j = 0; j < size_ef; j++) {
			if(i == j) {
				if(is_self_reachable(original, ef[i])) {
					printf("hey hey\n\n");
					add_edge(ret_val, ef[i], ef[i]);
					orient_edge(ret_val, ef[i], ef[i]);
				}
				continue;
			}
			if(is_reachable(original, ef[i], ef[j])) {
				add_edge(ret_val, ef[i], ef[j]);
				orient_edge(ret_val, ef[i], ef[j]);
			}

		}
	}
	//printf("after\n");
	//print_orientation(ret_val);
	
	if(!are_reach_relations_equal(original, ret_val, ef, size_ef)) {
		printf("Not equal!\n");
		print_orientation(original);
		print_orientation(ret_val);
	}
	

	return ret_val;
}

bool is_source_or_sink (struct Orientation *orient, int v) {
	// This returns true if a v *is non-adjacent to unoriented edges*
	// and is a sink or a source
	struct Node* p = orient->adj_list[v];
	if (p == NULL) return false;
	bool isSource = true;
	bool isSink = true;
	while (p != NULL) {
		if(p->orientation == 0) return false;
		if(p->orientation == -1) {
			isSource = false;
		}
		if(p->orientation == +1) {
			isSink = false;
		}
		p = p->next;
	}
	return isSource || isSink;
}
